#pragma once

#include "PluginTypes.h"
#include <filesystem>
#include <memory>

namespace nf {

class PluginModule {
public:
    explicit PluginModule(std::filesystem::path path);
    ~PluginModule();

    PluginModule(const PluginModule&) = delete;
    PluginModule& operator=(const PluginModule&) = delete;
    PluginModule(PluginModule&& other) noexcept;
    PluginModule& operator=(PluginModule&& other) noexcept;

    bool Load();
    void Unload();

    bool IsLoaded() const { return m_info.status == PluginStatus::Loaded; }
    const LoadedPluginInfo& GetInfo() const { return m_info; }
    const std::filesystem::path& GetPath() const { return m_path; }

private:
    std::filesystem::path m_path;
    LoadedPluginInfo m_info;
    void* m_hModule = nullptr;

    NF_GetPluginInfo_Fn m_fnGetInfo = nullptr;
    NF_RegisterOperators_Fn m_fnRegisterOps = nullptr;
    NF_UnloadPlugin_Fn m_fnUnload = nullptr;
};

} // namespace nf
