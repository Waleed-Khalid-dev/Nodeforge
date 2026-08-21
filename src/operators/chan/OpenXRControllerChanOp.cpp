#include "OpenXRControllerChanOp.h"
#include "../../xr/OpenXRRuntime.h"

namespace nf {

OpenXRControllerChanOp::OpenXRControllerChanOp(NodeId id, const std::string& name)
    : Node(id, name, "OpenXRControllerChanOp") {
    m_outLeftPin = AddOutputPin("out_left", PinType::Chan);
    m_outRightPin = AddOutputPin("out_right", PinType::Chan);

    SetParam("hand_select", static_cast<int32_t>(0)); // 0: Both, 1: Left, 2: Right
    SetParam("haptic_pulse", 0.0f);

    const std::vector<std::string> chanNames = {
        "tx", "ty", "tz", "rx", "ry", "rz", "rw",
        "trigger", "grip", "stick_x", "stick_y",
        "btn_primary", "btn_secondary", "btn_stick", "battery"
    };

    m_leftBuffer.Resize(chanNames.size(), 1);
    m_leftBuffer.SetChannelNames(chanNames);

    m_rightBuffer.Resize(chanNames.size(), 1);
    m_rightBuffer.SetChannelNames(chanNames);
}

bool OpenXRControllerChanOp::Cook(const CookContext& /*context*/) {
    auto& xr = OpenXRRuntime::Instance();
    const auto& left = xr.GetController(XRHand::Left);
    const auto& right = xr.GetController(XRHand::Right);

    auto fillBuffer = [](ChannelBuffer& buf, const XRControllerState& ctrl) {
        buf.GetChannelData(0)[0] = ctrl.position.x;
        buf.GetChannelData(1)[0] = ctrl.position.y;
        buf.GetChannelData(2)[0] = ctrl.position.z;
        buf.GetChannelData(3)[0] = ctrl.orientation.x;
        buf.GetChannelData(4)[0] = ctrl.orientation.y;
        buf.GetChannelData(5)[0] = ctrl.orientation.z;
        buf.GetChannelData(6)[0] = ctrl.orientation.w;
        buf.GetChannelData(7)[0] = ctrl.trigger;
        buf.GetChannelData(8)[0] = ctrl.grip;
        buf.GetChannelData(9)[0] = ctrl.thumbstick.x;
        buf.GetChannelData(10)[0] = ctrl.thumbstick.y;
        buf.GetChannelData(11)[0] = ctrl.primaryButton ? 1.0f : 0.0f;
        buf.GetChannelData(12)[0] = ctrl.secondaryButton ? 1.0f : 0.0f;
        buf.GetChannelData(13)[0] = ctrl.thumbstickClick ? 1.0f : 0.0f;
        buf.GetChannelData(14)[0] = ctrl.batteryLevel;
    };

    fillBuffer(m_leftBuffer, left);
    fillBuffer(m_rightBuffer, right);

    m_outLeftPin->SetValue(PinValue(m_leftBuffer));
    m_outRightPin->SetValue(PinValue(m_rightBuffer));
    return true;
}

} // namespace nf
