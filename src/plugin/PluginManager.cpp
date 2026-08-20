#include "PluginManager.h"
#include "PluginNodeProxy.h"
#include "../graph/NodeRegistry.h"
#include <spdlog/spdlog.h>
#include <cstdlib>
#include <sstream>

namespace nf {

PluginManager& PluginManager::Instance() {
    static PluginManager instance;
    return instance;
}

PluginManager::PluginManager() {
    // 1. Project local plugins folder
    AddSearchPath(std::filesystem::current_path() / "plugins");

    // 2. APPDATA / User plugins folder
#if defined(_WIN32)
    const char* appData = std::getenv("APPDATA");
    if (appData) {
        AddSearchPath(std::filesystem::path(appData) / "NodeForge" / "plugins");
    }
#else
    const char* home = std::getenv("HOME");
    if (home) {
        AddSearchPath(std::filesystem::path(home) / ".nodeforge" / "plugins");
    }
#endif

    // 3. Environment variable NF_PLUGIN_PATH
    const char* envPath = std::getenv("NF_PLUGIN_PATH");
    if (envPath) {
        std::string s(envPath);
        std::stringstream ss(s);
        std::string item;
#if defined(_WIN32)
        char delimiter = ';';
#else
        char delimiter = ':';
#endif
        while (std::getline(ss, item, delimiter)) {
            if (!item.empty()) {
                AddSearchPath(item);
            }
        }
    }
}

PluginManager::~PluginManager() {
    Clear();
}

void PluginManager::SetDefaultSearchPaths(const std::filesystem::path& appDir) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!appDir.empty()) {
        m_searchPaths.insert(m_searchPaths.begin(), appDir / "plugins");
    }
}

void PluginManager::AddSearchPath(const std::filesystem::path& path) {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (const auto& p : m_searchPaths) {
        if (p == path) return;
    }
    m_searchPaths.push_back(path);
}

std::vector<std::filesystem::path> PluginManager::GetSearchPaths() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_searchPaths;
}

uint32_t PluginManager::ScanAndLoadPlugins() {
    std::vector<std::filesystem::path> paths;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        paths = m_searchPaths;
    }

    uint32_t totalLoaded = 0;
    for (const auto& dirPath : paths) {
        if (!std::filesystem::exists(dirPath) || !std::filesystem::is_directory(dirPath)) {
            continue;
        }

        try {
            for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
                if (!entry.is_regular_file()) continue;

                auto ext = entry.path().extension().string();
#if defined(_WIN32)
                if (ext == ".dll")
#elif defined(__APPLE__)
                if (ext == ".dylib")
#else
                if (ext == ".so")
#endif
                {
                    if (LoadPlugin(entry.path())) {
                        totalLoaded++;
                    }
                }
            }
        } catch (const std::exception& e) {
            spdlog::warn("Error scanning plugin directory '{}': {}", dirPath.string(), e.what());
        }
    }

    return totalLoaded;
}

bool PluginManager::LoadPlugin(const std::filesystem::path& dllPath) {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto module = std::make_unique<PluginModule>(dllPath);
    if (!module->Load()) {
        spdlog::warn("PluginModule failed to load '{}'", dllPath.string());
        return false;
    }

    const auto& info = module->GetInfo();
    std::string pluginName = info.name;

    // If an existing plugin with same name was loaded, unload it first
    auto it = m_loadedModules.find(pluginName);
    if (it != m_loadedModules.end()) {
        it->second->Unload();
    }

    // Register all exported operators into NodeRegistry
    for (const auto& op : info.operators) {
        NodeTypeInfo nodeType;
        nodeType.typeName = op.typeName;
        nodeType.family = static_cast<NodeFamily>(op.family);
        nodeType.category = op.category;
        nodeType.description = op.description;
        nodeType.factory = [op](NodeId id, const std::string& name) -> std::unique_ptr<Node> {
            return std::make_unique<PluginNodeProxy>(id, name, op);
        };

        NodeRegistry::Instance().Register(nodeType);
        spdlog::info("Registered plugin operator '{}' [{}] from plugin '{}'",
            op.typeName, op.category, pluginName);
    }

    m_pathToPluginName[dllPath] = pluginName;
    m_loadedModules[pluginName] = std::move(module);
    return true;
}

bool PluginManager::UnloadPlugin(const std::string& pluginName) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_loadedModules.find(pluginName);
    if (it == m_loadedModules.end()) return false;

    it->second->Unload();
    m_loadedModules.erase(it);
    spdlog::info("Unloaded plugin '{}'", pluginName);
    return true;
}

bool PluginManager::ReloadPlugin(const std::string& pluginName) {
    std::filesystem::path path;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_loadedModules.find(pluginName);
        if (it == m_loadedModules.end()) return false;
        path = it->second->GetPath();
    }

    UnloadPlugin(pluginName);
    return LoadPlugin(path);
}

uint32_t PluginManager::ReloadAllPlugins() {
    std::vector<std::filesystem::path> paths;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (const auto& [name, module] : m_loadedModules) {
            paths.push_back(module->GetPath());
        }
    }

    Clear();

    uint32_t reloaded = 0;
    for (const auto& p : paths) {
        if (LoadPlugin(p)) {
            reloaded++;
        }
    }
    return reloaded;
}

std::vector<LoadedPluginInfo> PluginManager::GetLoadedPlugins() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<LoadedPluginInfo> list;
    list.reserve(m_loadedModules.size());
    for (const auto& [name, module] : m_loadedModules) {
        list.push_back(module->GetInfo());
    }
    return list;
}

const LoadedPluginInfo* PluginManager::GetPluginInfo(const std::string& pluginName) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_loadedModules.find(pluginName);
    if (it != m_loadedModules.end()) {
        return &it->second->GetInfo();
    }
    return nullptr;
}

size_t PluginManager::GetLoadedPluginCount() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_loadedModules.size();
}

void PluginManager::Clear() {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& [name, module] : m_loadedModules) {
        module->Unload();
    }
    m_loadedModules.clear();
    m_pathToPluginName.clear();
}

} // namespace nf
