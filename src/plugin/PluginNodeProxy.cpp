#include "PluginNodeProxy.h"
#include "../gpu/Texture.h"
#include "../core/ChannelBuffer.h"
#include "../core/DataTable.h"
#include <spdlog/spdlog.h>

namespace nf {

static PinType ConvertFromAbiPinType(NF_PinType type) {
    switch (type) {
        case NF_PIN_TYPE_TEXTURE2D: return PinType::Tex;
        case NF_PIN_TYPE_CHANNEL:   return PinType::Chan;
        case NF_PIN_TYPE_TABLE:     return PinType::Data;
        case NF_PIN_TYPE_GEOMETRY:  return PinType::Geom;
        case NF_PIN_TYPE_MATERIAL:  return PinType::Mat;
        default:                    return PinType::Any;
    }
}

PluginNodeProxy::PluginNodeProxy(NodeId id, const std::string& name, const PluginOperatorInfo& opInfo)
    : Node(id, name, opInfo.typeName)
    , m_opInfo(opInfo) {
    
    if (m_opInfo.vtable.createInstance) {
        try {
            m_instanceHandle = m_opInfo.vtable.createInstance(id, name.c_str());
        } catch (const std::exception& e) {
            spdlog::error("Exception creating plugin instance '{}': {}", name, e.what());
        } catch (...) {
            spdlog::error("Unknown exception creating plugin instance '{}'", name);
        }
    }

    InitPinsAndParams();
}

PluginNodeProxy::~PluginNodeProxy() {
    if (m_instanceHandle && m_opInfo.vtable.destroyInstance) {
        try {
            m_opInfo.vtable.destroyInstance(m_instanceHandle);
        } catch (...) {}
        m_instanceHandle = nullptr;
    }
}

void PluginNodeProxy::InitPinsAndParams() {
    // 1. Pins
    for (const auto& pinDef : m_opInfo.pinDefs) {
        if (!pinDef.name) continue;
        PinType pt = ConvertFromAbiPinType(pinDef.type);
        if (pinDef.direction == NF_PIN_DIR_INPUT) {
            AddInputPin(pinDef.name, pt);
        } else {
            AddOutputPin(pinDef.name, pt);
        }
    }

    // 2. Parameters
    for (const auto& paramDef : m_opInfo.paramDefs) {
        if (!paramDef.name) continue;
        std::string pName = paramDef.name;
        std::string pLabel = paramDef.label ? paramDef.label : pName;
        std::string pCat = paramDef.category ? paramDef.category : "General";

        switch (paramDef.type) {
            case NF_PARAM_FLOAT:
                m_params.AddFloat(pName, pLabel, paramDef.defaultFloat, paramDef.minVal, paramDef.maxVal, (paramDef.minVal != paramDef.maxVal), pCat);
                break;
            case NF_PARAM_INT:
                m_params.AddInt(pName, pLabel, paramDef.defaultInt, static_cast<int32_t>(paramDef.minVal), static_cast<int32_t>(paramDef.maxVal), (paramDef.minVal != paramDef.maxVal), pCat);
                break;
            case NF_PARAM_BOOL:
                m_params.AddBool(pName, pLabel, paramDef.defaultBool, pCat);
                break;
            case NF_PARAM_STRING:
                m_params.AddString(pName, pLabel, paramDef.defaultString ? paramDef.defaultString : "", pCat);
                break;
            case NF_PARAM_MENU: {
                std::vector<std::string> items;
                if (paramDef.menuItems && paramDef.menuItemCount > 0) {
                    for (uint32_t k = 0; k < paramDef.menuItemCount; ++k) {
                        if (paramDef.menuItems[k]) items.push_back(paramDef.menuItems[k]);
                    }
                }
                m_params.AddMenu(pName, pLabel, paramDef.defaultInt, items, pCat);
                break;
            }
            default:
                m_params.AddFloat(pName, pLabel, paramDef.defaultFloat, 0.0f, 1.0f, false, pCat);
                break;
        }
    }
}

void PluginNodeProxy::SyncParamsToPlugin() {
    if (!m_instanceHandle) return;

    for (const auto& param : m_params.GetAll()) {
        const std::string& pName = param->GetName();
        const auto& pVal = param->GetValue();

        if (pVal.Is<float>() && m_opInfo.vtable.setParamFloat) {
            m_opInfo.vtable.setParamFloat(m_instanceHandle, pName.c_str(), pVal.Get<float>());
        } else if (pVal.Is<int32_t>() && m_opInfo.vtable.setParamInt) {
            m_opInfo.vtable.setParamInt(m_instanceHandle, pName.c_str(), pVal.Get<int32_t>());
        } else if (pVal.Is<bool>() && m_opInfo.vtable.setParamBool) {
            m_opInfo.vtable.setParamBool(m_instanceHandle, pName.c_str(), pVal.Get<bool>());
        } else if (pVal.Is<std::string>() && m_opInfo.vtable.setParamString) {
            m_opInfo.vtable.setParamString(m_instanceHandle, pName.c_str(), pVal.Get<std::string>().c_str());
        }
    }
}

bool PluginNodeProxy::Cook(const CookContext& context) {
    if (!m_instanceHandle || !m_opInfo.vtable.cook) {
        return false;
    }

    SyncParamsToPlugin();

    NF_CookContext nfCtx{};
    nfCtx.frameNumber = context.frameIndex;
    nfCtx.timeSeconds = context.timeSeconds;
    nfCtx.deltaTime = context.deltaTimeSeconds;
    nfCtx.isRealtime = true;

    // 1. Set Inputs
    if (m_opInfo.family == NF_NODE_FAMILY_CHANOP && m_opInfo.vtable.setInputChannels) {
        for (size_t i = 0; i < m_inputPins.size(); ++i) {
            const auto& pin = m_inputPins[i];
            if (pin->IsConnected() && pin->GetValue().Is<ChannelBuffer>()) {
                const auto& chanBuf = pin->GetValue().Get<ChannelBuffer>();
                NF_ChannelBufferDescriptor desc{};
                desc.channelCount = static_cast<uint32_t>(chanBuf.GetChannelCount());
                desc.sampleCount = static_cast<uint32_t>(chanBuf.GetSampleCount());
                desc.sampleRate = chanBuf.GetSampleRate();
                m_opInfo.vtable.setInputChannels(m_instanceHandle, static_cast<uint32_t>(i), &desc);
            }
        }
    } else if (m_opInfo.family == NF_NODE_FAMILY_DATAOP && m_opInfo.vtable.setInputTable) {
        for (size_t i = 0; i < m_inputPins.size(); ++i) {
            const auto& pin = m_inputPins[i];
            if (pin->IsConnected() && pin->GetValue().Is<DataTable>()) {
                const auto& dt = pin->GetValue().Get<DataTable>();
                NF_DataTableDescriptor desc{};
                desc.rowCount = static_cast<uint32_t>(dt.GetRowCount());
                desc.colCount = static_cast<uint32_t>(dt.GetColumnCount());
                m_opInfo.vtable.setInputTable(m_instanceHandle, static_cast<uint32_t>(i), &desc);
            }
        }
    }

    // 2. Execute Cook
    NF_Result res = NF_ERROR_EXECUTION_FAILED;
    try {
        res = m_opInfo.vtable.cook(m_instanceHandle, &nfCtx);
    } catch (const std::exception& e) {
        spdlog::error("Exception in plugin cook '{}': {}", m_name, e.what());
        return false;
    } catch (...) {
        spdlog::error("Unknown exception in plugin cook '{}'", m_name);
        return false;
    }

    if (res != NF_SUCCESS) {
        return false;
    }

    // 3. Retrieve Outputs
    if (m_opInfo.family == NF_NODE_FAMILY_CHANOP && m_opInfo.vtable.getOutputChannels) {
        for (size_t i = 0; i < m_outputPins.size(); ++i) {
            NF_ChannelBufferDescriptor desc{};
            if (m_opInfo.vtable.getOutputChannels(m_instanceHandle, static_cast<uint32_t>(i), &desc) == NF_SUCCESS) {
                if (desc.channelCount > 0 && desc.sampleCount > 0 && desc.channelData) {
                    std::vector<std::string> names;
                    if (desc.channelNames) {
                        for (uint32_t k = 0; k < desc.channelCount; ++k) {
                            names.push_back(desc.channelNames[k] ? desc.channelNames[k] : ("chan" + std::to_string(k)));
                        }
                    } else {
                        for (uint32_t k = 0; k < desc.channelCount; ++k) {
                            names.push_back("chan" + std::to_string(k));
                        }
                    }
                    ChannelBuffer outBuf(names, desc.sampleCount, desc.sampleRate > 0 ? desc.sampleRate : 60.0);
                    for (uint32_t ch = 0; ch < desc.channelCount; ++ch) {
                        if (desc.channelData[ch]) {
                            std::memcpy(outBuf.GetChannelData(ch), desc.channelData[ch], desc.sampleCount * sizeof(float));
                        }
                    }
                    m_outputPins[i]->SetValue(PinValue(std::move(outBuf)));
                }
            }
        }
    } else if (m_opInfo.family == NF_NODE_FAMILY_DATAOP && m_opInfo.vtable.getOutputTable) {
        for (size_t i = 0; i < m_outputPins.size(); ++i) {
            NF_DataTableDescriptor desc{};
            if (m_opInfo.vtable.getOutputTable(m_instanceHandle, static_cast<uint32_t>(i), &desc) == NF_SUCCESS) {
                DataTable dt(desc.rowCount, desc.colCount);
                if (desc.cells) {
                    for (uint32_t r = 0; r < desc.rowCount; ++r) {
                        for (uint32_t c = 0; c < desc.colCount; ++c) {
                            if (desc.cells[r] && desc.cells[r][c]) {
                                dt.SetCell(r, c, desc.cells[r][c]);
                            }
                        }
                    }
                }
                m_outputPins[i]->SetValue(PinValue(std::move(dt)));
            }
        }
    }

    return true;
}

} // namespace nf
