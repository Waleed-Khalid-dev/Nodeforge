#pragma once

#include "../../sdk/include/nf_plugin_abi.h"
#include <string>
#include <vector>
#include <memory>
#include <filesystem>

namespace nf {

enum class PluginStatus {
    Unloaded,
    Loaded,
    Error,
    IncompatibleABI
};

struct PluginOperatorInfo {
    std::string typeName;
    NF_NodeFamily family;
    std::string category;
    std::string description;
    std::string author;
    uint32_t version = 1;
    std::vector<NF_PinDef> pinDefs;
    std::vector<NF_ParamDef> paramDefs;
    NF_NodeVTable vtable{};
};

struct LoadedPluginInfo {
    std::filesystem::path filePath;
    std::string name;
    std::string author;
    std::string description;
    std::string url;
    std::string versionString;
    uint32_t versionMajor = 0;
    uint32_t versionMinor = 0;
    uint32_t versionPatch = 0;
    uint32_t abiVersion = 0;
    PluginStatus status = PluginStatus::Unloaded;
    std::string errorMessage;
    std::vector<PluginOperatorInfo> operators;
    void* nativeHandle = nullptr; // HMODULE on Windows
};

} // namespace nf
