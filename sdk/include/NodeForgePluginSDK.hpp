#pragma once

#include "nf_plugin_abi.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <stdexcept>
#include <iostream>
#include <cstring>

namespace nf::sdk {

// ─── Base Plugin Node ────────────────────────────────────────────────────────
class PluginNode {
public:
    PluginNode(uint64_t nodeId, std::string name)
        : m_nodeId(nodeId), m_name(std::move(name)) {}
    virtual ~PluginNode() = default;

    uint64_t GetId() const noexcept { return m_nodeId; }
    const std::string& GetName() const noexcept { return m_name; }

    virtual NF_Result Cook(const NF_CookContext& ctx) = 0;
    virtual void OnParamChanged(const std::string& /*paramName*/) {}

    // Parameter setters from host
    void SetParamFloat(const std::string& name, float val) { m_floatParams[name] = val; OnParamChanged(name); }
    void SetParamInt(const std::string& name, int32_t val) { m_intParams[name] = val; OnParamChanged(name); }
    void SetParamBool(const std::string& name, bool val) { m_boolParams[name] = val; OnParamChanged(name); }
    void SetParamString(const std::string& name, std::string val) { m_stringParams[name] = std::move(val); OnParamChanged(name); }

    // Parameter getters for subclass
    float GetFloat(const std::string& name, float fallback = 0.0f) const {
        auto it = m_floatParams.find(name);
        return it != m_floatParams.end() ? it->second : fallback;
    }
    int32_t GetInt(const std::string& name, int32_t fallback = 0) const {
        auto it = m_intParams.find(name);
        return it != m_intParams.end() ? it->second : fallback;
    }
    bool GetBool(const std::string& name, bool fallback = false) const {
        auto it = m_boolParams.find(name);
        return it != m_boolParams.end() ? it->second : fallback;
    }
    const std::string& GetString(const std::string& name, const std::string& fallback = "") const {
        auto it = m_stringParams.find(name);
        return it != m_stringParams.end() ? it->second : fallback;
    }

protected:
    uint64_t m_nodeId;
    std::string m_name;
    std::unordered_map<std::string, float> m_floatParams;
    std::unordered_map<std::string, int32_t> m_intParams;
    std::unordered_map<std::string, bool> m_boolParams;
    std::unordered_map<std::string, std::string> m_stringParams;
};

// ─── TexOp Plugin Base (GPU TOP) ─────────────────────────────────────────────
class TexOpPlugin : public PluginNode {
public:
    using PluginNode::PluginNode;

    virtual NF_Result SetInputTexture(uint32_t index, const NF_VulkanTextureDescriptor& tex) {
        if (index >= m_inputTex.size()) {
            m_inputTex.resize(index + 1);
        }
        m_inputTex[index] = tex;
        return NF_SUCCESS;
    }

    virtual NF_Result GetOutputTexture(uint32_t index, NF_VulkanTextureDescriptor& outTex) {
        if (index < m_outputTex.size()) {
            outTex = m_outputTex[index];
            return NF_SUCCESS;
        }
        return NF_ERROR_INVALID_ARG;
    }

protected:
    std::vector<NF_VulkanTextureDescriptor> m_inputTex;
    std::vector<NF_VulkanTextureDescriptor> m_outputTex;
};

// ─── ChanOp Plugin Base (Channels/Audio CHOP) ────────────────────────────────
class ChanOpPlugin : public PluginNode {
public:
    using PluginNode::PluginNode;

    virtual NF_Result SetInputChannels(uint32_t index, const NF_ChannelBufferDescriptor& chan) {
        if (index >= m_inputChannels.size()) {
            m_inputChannels.resize(index + 1);
        }
        m_inputChannels[index] = chan;
        return NF_SUCCESS;
    }

    virtual NF_Result GetOutputChannels(uint32_t index, NF_ChannelBufferDescriptor& outChan) {
        if (index < m_outputChannels.size()) {
            outChan = m_outputChannels[index];
            return NF_SUCCESS;
        }
        return NF_ERROR_INVALID_ARG;
    }

protected:
    std::vector<NF_ChannelBufferDescriptor> m_inputChannels;
    std::vector<NF_ChannelBufferDescriptor> m_outputChannels;
};

// ─── DataOp Plugin Base (Tables/Text DAT) ────────────────────────────────────
class DataOpPlugin : public PluginNode {
public:
    using PluginNode::PluginNode;

    virtual NF_Result SetInputTable(uint32_t index, const NF_DataTableDescriptor& table) {
        if (index >= m_inputTables.size()) {
            m_inputTables.resize(index + 1);
        }
        m_inputTables[index] = table;
        return NF_SUCCESS;
    }

    virtual NF_Result GetOutputTable(uint32_t index, NF_DataTableDescriptor& outTable) {
        if (index < m_outputTables.size()) {
            outTable = m_outputTables[index];
            return NF_SUCCESS;
        }
        return NF_ERROR_INVALID_ARG;
    }

protected:
    std::vector<NF_DataTableDescriptor> m_inputTables;
    std::vector<NF_DataTableDescriptor> m_outputTables;
};

// ─── VTable Binding Helpers with Exception Isolation ─────────────────────────
template <typename T>
struct VTableBinder {
    static NF_PluginInstanceHandle CreateInstance(uint64_t id, const char* name) {
        try {
            return static_cast<NF_PluginInstanceHandle>(new T(id, name ? name : ""));
        } catch (...) {
            return nullptr;
        }
    }

    static void DestroyInstance(NF_PluginInstanceHandle handle) {
        if (handle) {
            delete static_cast<T*>(handle);
        }
    }

    static NF_Result Cook(NF_PluginInstanceHandle handle, const NF_CookContext* ctx) {
        if (!handle || !ctx) return NF_ERROR_INVALID_ARG;
        try {
            return static_cast<T*>(handle)->Cook(*ctx);
        } catch (...) {
            return NF_ERROR_EXECUTION_FAILED;
        }
    }

    static void OnParamChanged(NF_PluginInstanceHandle handle, const char* name) {
        if (!handle || !name) return;
        try {
            static_cast<T*>(handle)->OnParamChanged(name);
        } catch (...) {}
    }

    static NF_Result GetOutputTexture(NF_PluginInstanceHandle handle, uint32_t pinIndex, NF_VulkanTextureDescriptor* outTex) {
        if (!handle || !outTex) return NF_ERROR_INVALID_ARG;
        if constexpr (std::is_base_of_v<TexOpPlugin, T>) {
            try {
                return static_cast<T*>(handle)->GetOutputTexture(pinIndex, *outTex);
            } catch (...) {
                return NF_ERROR_EXECUTION_FAILED;
            }
        }
        return NF_ERROR_UNSUPPORTED;
    }

    static NF_Result SetInputTexture(NF_PluginInstanceHandle handle, uint32_t pinIndex, const NF_VulkanTextureDescriptor* inTex) {
        if (!handle || !inTex) return NF_ERROR_INVALID_ARG;
        if constexpr (std::is_base_of_v<TexOpPlugin, T>) {
            try {
                return static_cast<T*>(handle)->SetInputTexture(pinIndex, *inTex);
            } catch (...) {
                return NF_ERROR_EXECUTION_FAILED;
            }
        }
        return NF_ERROR_UNSUPPORTED;
    }

    static NF_Result GetOutputChannels(NF_PluginInstanceHandle handle, uint32_t pinIndex, NF_ChannelBufferDescriptor* outChan) {
        if (!handle || !outChan) return NF_ERROR_INVALID_ARG;
        if constexpr (std::is_base_of_v<ChanOpPlugin, T>) {
            try {
                return static_cast<T*>(handle)->GetOutputChannels(pinIndex, *outChan);
            } catch (...) {
                return NF_ERROR_EXECUTION_FAILED;
            }
        }
        return NF_ERROR_UNSUPPORTED;
    }

    static NF_Result SetInputChannels(NF_PluginInstanceHandle handle, uint32_t pinIndex, const NF_ChannelBufferDescriptor* inChan) {
        if (!handle || !inChan) return NF_ERROR_INVALID_ARG;
        if constexpr (std::is_base_of_v<ChanOpPlugin, T>) {
            try {
                return static_cast<T*>(handle)->SetInputChannels(pinIndex, *inChan);
            } catch (...) {
                return NF_ERROR_EXECUTION_FAILED;
            }
        }
        return NF_ERROR_UNSUPPORTED;
    }

    static NF_Result GetOutputTable(NF_PluginInstanceHandle handle, uint32_t pinIndex, NF_DataTableDescriptor* outTable) {
        if (!handle || !outTable) return NF_ERROR_INVALID_ARG;
        if constexpr (std::is_base_of_v<DataOpPlugin, T>) {
            try {
                return static_cast<T*>(handle)->GetOutputTable(pinIndex, *outTable);
            } catch (...) {
                return NF_ERROR_EXECUTION_FAILED;
            }
        }
        return NF_ERROR_UNSUPPORTED;
    }

    static NF_Result SetInputTable(NF_PluginInstanceHandle handle, uint32_t pinIndex, const NF_DataTableDescriptor* inTable) {
        if (!handle || !inTable) return NF_ERROR_INVALID_ARG;
        if constexpr (std::is_base_of_v<DataOpPlugin, T>) {
            try {
                return static_cast<T*>(handle)->SetInputTable(pinIndex, *inTable);
            } catch (...) {
                return NF_ERROR_EXECUTION_FAILED;
            }
        }
        return NF_ERROR_UNSUPPORTED;
    }

    static void SetParamFloat(NF_PluginInstanceHandle handle, const char* name, float value) {
        if (handle && name) static_cast<T*>(handle)->SetParamFloat(name, value);
    }
    static void SetParamInt(NF_PluginInstanceHandle handle, const char* name, int32_t value) {
        if (handle && name) static_cast<T*>(handle)->SetParamInt(name, value);
    }
    static void SetParamBool(NF_PluginInstanceHandle handle, const char* name, bool value) {
        if (handle && name) static_cast<T*>(handle)->SetParamBool(name, value);
    }
    static void SetParamString(NF_PluginInstanceHandle handle, const char* name, const char* value) {
        if (handle && name && value) static_cast<T*>(handle)->SetParamString(name, value);
    }

    static NF_NodeVTable MakeVTable() {
        NF_NodeVTable vt{};
        vt.createInstance    = &CreateInstance;
        vt.destroyInstance   = &DestroyInstance;
        vt.cook              = &Cook;
        vt.onParamChanged    = &OnParamChanged;
        vt.getOutputTexture  = &GetOutputTexture;
        vt.setInputTexture   = &SetInputTexture;
        vt.getOutputChannels = &GetOutputChannels;
        vt.setInputChannels  = &SetInputChannels;
        vt.getOutputTable    = &GetOutputTable;
        vt.setInputTable     = &SetInputTable;
        vt.setParamFloat     = &SetParamFloat;
        vt.setParamInt       = &SetParamInt;
        vt.setParamBool      = &SetParamBool;
        vt.setParamString    = &SetParamString;
        return vt;
    }
};

} // namespace nf::sdk
