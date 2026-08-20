#pragma once

#include "PluginTypes.h"
#include "PluginModule.h"
#include <vector>
#include <unordered_map>
#include <memory>
#include <filesystem>
#include <mutex>

namespace nf {

class PluginManager {
public:
    static PluginManager& Instance();

    // Search Paths
    void AddSearchPath(const std::filesystem::path& path);
    std::vector<std::filesystem::path> GetSearchPaths() const;
    void SetDefaultSearchPaths(const std::filesystem::path& appDir);

    // Discovery & Loading
    uint32_t ScanAndLoadPlugins();
    bool LoadPlugin(const std::filesystem::path& dllPath);
    bool UnloadPlugin(const std::string& pluginName);
    bool ReloadPlugin(const std::string& pluginName);
    uint32_t ReloadAllPlugins();

    // Inspection
    std::vector<LoadedPluginInfo> GetLoadedPlugins() const;
    const LoadedPluginInfo* GetPluginInfo(const std::string& pluginName) const;
    size_t GetLoadedPluginCount() const;

    void Clear();

private:
    PluginManager();
    ~PluginManager();

    mutable std::mutex m_mutex;
    std::vector<std::filesystem::path> m_searchPaths;
    std::unordered_map<std::string, std::unique_ptr<PluginModule>> m_loadedModules; // pluginName -> Module
    std::unordered_map<std::filesystem::path, std::string> m_pathToPluginName;
};

} // namespace nf
