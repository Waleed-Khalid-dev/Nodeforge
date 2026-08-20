#include "PluginModule.h"
#include <spdlog/spdlog.h>
#include <fmt/format.h>

#if defined(_WIN32)
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
#else
    #include <dlfcn.h>
#endif

namespace nf {

PluginModule::PluginModule(std::filesystem::path path)
    : m_path(std::move(path)) {
    m_info.filePath = m_path;
}

PluginModule::~PluginModule() {
    Unload();
}

PluginModule::PluginModule(PluginModule&& other) noexcept
    : m_path(std::move(other.m_path))
    , m_info(std::move(other.m_info))
    , m_hModule(other.m_hModule)
    , m_fnGetInfo(other.m_fnGetInfo)
    , m_fnRegisterOps(other.m_fnRegisterOps)
    , m_fnUnload(other.m_fnUnload) {
    other.m_hModule = nullptr;
    other.m_fnGetInfo = nullptr;
    other.m_fnRegisterOps = nullptr;
    other.m_fnUnload = nullptr;
}

PluginModule& PluginModule::operator=(PluginModule&& other) noexcept {
    if (this != &other) {
        Unload();
        m_path = std::move(other.m_path);
        m_info = std::move(other.m_info);
        m_hModule = other.m_hModule;
        m_fnGetInfo = other.m_fnGetInfo;
        m_fnRegisterOps = other.m_fnRegisterOps;
        m_fnUnload = other.m_fnUnload;

        other.m_hModule = nullptr;
        other.m_fnGetInfo = nullptr;
        other.m_fnRegisterOps = nullptr;
        other.m_fnUnload = nullptr;
    }
    return *this;
}

bool PluginModule::Load() {
    Unload();

    std::string pathStr = m_path.string();
#if defined(_WIN32)
    HMODULE hMod = LoadLibraryExA(pathStr.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    if (!hMod) {
        DWORD err = GetLastError();
        m_info.status = PluginStatus::Error;
        m_info.errorMessage = fmt::format("LoadLibraryExA failed with error code: {}", err);
        spdlog::error("Failed to load plugin DLL '{}': {}", pathStr, m_info.errorMessage);
        return false;
    }
    m_hModule = static_cast<void*>(hMod);

    m_fnGetInfo = reinterpret_cast<NF_GetPluginInfo_Fn>(GetProcAddress(hMod, NF_ENTRYPOINT_GET_PLUGIN_INFO));
    m_fnRegisterOps = reinterpret_cast<NF_RegisterOperators_Fn>(GetProcAddress(hMod, NF_ENTRYPOINT_REGISTER_OPERATORS));
    m_fnUnload = reinterpret_cast<NF_UnloadPlugin_Fn>(GetProcAddress(hMod, NF_ENTRYPOINT_UNLOAD_PLUGIN));
#else
    void* hMod = dlopen(pathStr.c_str(), RTLD_NOW | RTLD_LOCAL);
    if (!hMod) {
        m_info.status = PluginStatus::Error;
        m_info.errorMessage = dlerror();
        spdlog::error("Failed to load plugin shared lib '{}': {}", pathStr, m_info.errorMessage);
        return false;
    }
    m_hModule = hMod;

    m_fnGetInfo = reinterpret_cast<NF_GetPluginInfo_Fn>(dlsym(hMod, NF_ENTRYPOINT_GET_PLUGIN_INFO));
    m_fnRegisterOps = reinterpret_cast<NF_RegisterOperators_Fn>(dlsym(hMod, NF_ENTRYPOINT_REGISTER_OPERATORS));
    m_fnUnload = reinterpret_cast<NF_UnloadPlugin_Fn>(dlsym(hMod, NF_ENTRYPOINT_UNLOAD_PLUGIN));
#endif

    if (!m_fnGetInfo || !m_fnRegisterOps) {
        m_info.status = PluginStatus::Error;
        m_info.errorMessage = "Missing mandatory entrypoints (NF_GetPluginInfo or NF_RegisterOperators)";
        spdlog::error("Plugin '{}' validation failed: {}", pathStr, m_info.errorMessage);
        Unload();
        return false;
    }

    const NF_PluginInfo* pInfo = m_fnGetInfo();
    if (!pInfo) {
        m_info.status = PluginStatus::Error;
        m_info.errorMessage = "NF_GetPluginInfo returned NULL";
        spdlog::error("Plugin '{}' failed: {}", pathStr, m_info.errorMessage);
        Unload();
        return false;
    }

    m_info.name = pInfo->pluginName ? pInfo->pluginName : m_path.stem().string();
    m_info.author = pInfo->author ? pInfo->author : "Unknown";
    m_info.description = pInfo->description ? pInfo->description : "";
    m_info.url = pInfo->url ? pInfo->url : "";
    m_info.versionMajor = pInfo->versionMajor;
    m_info.versionMinor = pInfo->versionMinor;
    m_info.versionPatch = pInfo->versionPatch;
    m_info.versionString = fmt::format("{}.{}.{}", pInfo->versionMajor, pInfo->versionMinor, pInfo->versionPatch);
    m_info.abiVersion = pInfo->abiVersion;
    m_info.nativeHandle = m_hModule;

    // Validate ABI version
    if ((pInfo->abiVersion >> 16) != NF_PLUGIN_ABI_VERSION_MAJOR) {
        m_info.status = PluginStatus::IncompatibleABI;
        m_info.errorMessage = fmt::format("ABI Major Version Mismatch: Host {} != Plugin {}",
            NF_PLUGIN_ABI_VERSION_MAJOR, (pInfo->abiVersion >> 16));
        spdlog::warn("Plugin '{}' rejected: {}", m_info.name, m_info.errorMessage);
        return false;
    }

    // Retrieve operators
    uint32_t opCount = pInfo->operatorCount > 0 ? pInfo->operatorCount : 32;
    std::vector<NF_OperatorDef> rawOps(opCount);
    int32_t registeredCount = m_fnRegisterOps(rawOps.data(), opCount);

    if (registeredCount <= 0) {
        m_info.status = PluginStatus::Error;
        m_info.errorMessage = "NF_RegisterOperators returned 0 operators";
        spdlog::warn("Plugin '{}' registered 0 operators", m_info.name);
        return false;
    }

    m_info.operators.clear();
    m_info.operators.reserve(registeredCount);

    for (int32_t i = 0; i < registeredCount; ++i) {
        const auto& op = rawOps[i];
        if (!op.typeName || !op.vtable.cook) continue;

        PluginOperatorInfo opInfo;
        opInfo.typeName = op.typeName;
        opInfo.family = op.family;
        opInfo.category = op.category ? op.category : "Custom/Plugin";
        opInfo.description = op.description ? op.description : "";
        opInfo.author = op.author ? op.author : m_info.author;
        opInfo.version = op.version > 0 ? op.version : 1;
        opInfo.vtable = op.vtable;

        if (op.pinDefs && op.pinDefCount > 0) {
            opInfo.pinDefs.assign(op.pinDefs, op.pinDefs + op.pinDefCount);
        }
        if (op.paramDefs && op.paramDefCount > 0) {
            opInfo.paramDefs.assign(op.paramDefs, op.paramDefs + op.paramDefCount);
        }

        m_info.operators.push_back(std::move(opInfo));
    }

    m_info.status = PluginStatus::Loaded;
    m_info.errorMessage.clear();
    spdlog::info("Loaded plugin '{}' v{} with {} operators from '{}'",
        m_info.name, m_info.versionString, m_info.operators.size(), pathStr);

    return true;
}

void PluginModule::Unload() {
    if (m_hModule) {
        if (m_fnUnload) {
            try {
                m_fnUnload();
            } catch (...) {}
        }
#if defined(_WIN32)
        FreeLibrary(static_cast<HMODULE>(m_hModule));
#else
        dlclose(m_hModule);
#endif
        m_hModule = nullptr;
    }
    m_fnGetInfo = nullptr;
    m_fnRegisterOps = nullptr;
    m_fnUnload = nullptr;
    m_info.status = PluginStatus::Unloaded;
    m_info.operators.clear();
}

} // namespace nf
