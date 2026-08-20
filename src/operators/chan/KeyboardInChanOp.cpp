#include "KeyboardInChanOp.h"
#include "io/input/InputManager.h"
#include <sstream>
#include <vector>

namespace nf {

KeyboardInChanOp::KeyboardInChanOp(NodeId id, const std::string& name)
    : ChanOp(id, name, "KeyboardInChanOp") {
    m_outPin = AddOutputPin("output", PinType::Chan);

    SetParam("keys", std::string("space enter escape left right up down w a s d 1 2 3"));
    SetParam("include_modifiers", true);
    SetParam("include_pulses", true);
    SetParam("active", true);
}

KeyboardInChanOp::~KeyboardInChanOp() = default;

bool KeyboardInChanOp::Cook(const CookContext& /*context*/) {
    bool active = true;
    if (GetParam("active").Is<bool>()) {
        active = GetParam("active").Get<bool>();
    }

    std::string keysStr = "space enter escape";
    if (GetParam("keys").Is<std::string>()) {
        keysStr = GetParam("keys").Get<std::string>();
    }

    bool incModifiers = true;
    if (GetParam("include_modifiers").Is<bool>()) {
        incModifiers = GetParam("include_modifiers").Get<bool>();
    }

    bool incPulses = true;
    if (GetParam("include_pulses").Is<bool>()) {
        incPulses = GetParam("include_pulses").Get<bool>();
    }

    if (keysStr != m_cachedKeysStr || m_cachedChanNames.empty()) {
        m_cachedKeysStr = keysStr;
        m_cachedKeyList.clear();
        m_cachedChanNames.clear();

        std::stringstream ss(keysStr);
        std::string item;
        while (ss >> item) {
            m_cachedKeyList.push_back(item);
        }

        for (const auto& k : m_cachedKeyList) {
            m_cachedChanNames.push_back("key_" + k);
            if (incPulses) {
                m_cachedChanNames.push_back("pulse_" + k);
            }
        }

        if (incModifiers) {
            m_cachedChanNames.push_back("ctrl");
            m_cachedChanNames.push_back("shift");
            m_cachedChanNames.push_back("alt");
            m_cachedChanNames.push_back("meta");
        }

        if (m_cachedChanNames.empty()) {
            m_cachedChanNames.push_back("key_space");
        }
    }

    auto& input = InputManager::Instance();
    ChannelBuffer buf(m_cachedChanNames, 1, 60.0f);

    if (active) {
        size_t idx = 0;
        for (const auto& k : m_cachedKeyList) {
            float* pKey = buf.GetChannelData(idx++);
            if (pKey) *pKey = input.IsKeyDown(k) ? 1.0f : 0.0f;

            if (incPulses) {
                float* pPulse = buf.GetChannelData(idx++);
                if (pPulse) *pPulse = input.IsKeyPulse(k) ? 1.0f : 0.0f;
            }
        }

        if (incModifiers) {
            float* pCtrl = buf.GetChannelData(idx++);
            if (pCtrl) *pCtrl = input.IsCtrlDown() ? 1.0f : 0.0f;

            float* pShift = buf.GetChannelData(idx++);
            if (pShift) *pShift = input.IsShiftDown() ? 1.0f : 0.0f;

            float* pAlt = buf.GetChannelData(idx++);
            if (pAlt) *pAlt = input.IsAltDown() ? 1.0f : 0.0f;

            float* pMeta = buf.GetChannelData(idx++);
            if (pMeta) *pMeta = input.IsMetaDown() ? 1.0f : 0.0f;
        }
    }

    SetOutputBuffer(buf);
    return true;
}

} // namespace nf
