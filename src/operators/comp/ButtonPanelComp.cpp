#include "ButtonPanelComp.h"

namespace nf {

ButtonPanelComp::ButtonPanelComp(NodeId id, const std::string& name)
    : Node(id, name, "ButtonPanelComp") {
    m_inChanPin = AddInputPin("in_chan", PinType::Chan);
    m_outCompPin = AddOutputPin("output", PinType::Any);
    m_outChanPin = AddOutputPin("out_chan", PinType::Chan);

    SetParam("label", std::string("Trigger"));
    SetParam("button_type", static_cast<int32_t>(0)); // 0: Momentary, 1: Toggle, 2: Radio
    SetParam("state", false);
    SetParam("group_id", static_cast<int32_t>(0));
    SetParam("active_color", glm::vec4(0.2f, 0.9f, 0.4f, 1.0f));

    m_outBuffer.Resize(2, 1);
    m_outBuffer.SetChannelNames({"state", "pressed"});
    m_outBuffer.GetChannelData(0)[0] = 0.0f;
    m_outBuffer.GetChannelData(1)[0] = 0.0f;
}

bool ButtonPanelComp::GetState() const {
    return GetParam("state").Is<bool>() ? GetParam("state").Get<bool>() : false;
}

void ButtonPanelComp::Click() {
    int32_t type = GetParam("button_type").Is<int32_t>() ? GetParam("button_type").Get<int32_t>() : 0;
    if (type == 1) {
        // Toggle Latch
        bool current = GetState();
        SetParam("state", !current);
    } else if (type == 2) {
        // Radio group
        SetParam("state", true);
    }
}

void ButtonPanelComp::SetPressed(bool pressed) {
    m_isPressed = pressed;
    int32_t type = GetParam("button_type").Is<int32_t>() ? GetParam("button_type").Get<int32_t>() : 0;
    if (type == 0) {
        SetParam("state", pressed);
    }
}

bool ButtonPanelComp::Cook(const CookContext& /*context*/) {
    if (m_inChanPin && m_inChanPin->GetValue().Is<ChannelBuffer>()) {
        const auto& inBuf = m_inChanPin->GetValue().Get<ChannelBuffer>();
        if (inBuf.GetChannelCount() > 0 && inBuf.GetSampleCount() > 0) {
            bool inState = inBuf.GetChannelData(0)[0] > 0.5f;
            SetParam("state", inState);
        }
    }

    bool state = GetState();

    m_outBuffer.Resize(2, 1);
    m_outBuffer.SetChannelNames({"state", "pressed"});
    m_outBuffer.GetChannelData(0)[0] = state ? 1.0f : 0.0f;
    m_outBuffer.GetChannelData(1)[0] = m_isPressed ? 1.0f : 0.0f;

    m_outChanPin->SetValue(PinValue(m_outBuffer));
    return true;
}

} // namespace nf
