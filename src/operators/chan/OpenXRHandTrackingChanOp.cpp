#include "OpenXRHandTrackingChanOp.h"
#include "../../xr/OpenXRRuntime.h"

namespace nf {

OpenXRHandTrackingChanOp::OpenXRHandTrackingChanOp(NodeId id, const std::string& name)
    : Node(id, name, "OpenXRHandTrackingChanOp") {
    m_outSkeletonPin = AddOutputPin("out_skeleton", PinType::Chan);
    m_outGesturesPin = AddOutputPin("out_gestures", PinType::Chan);

    SetParam("hand_select", static_cast<int32_t>(0)); // 0: Both, 1: Left, 2: Right
    SetParam("confidence_threshold", 0.5f);

    m_skeletonBuffer.Resize(26 * 3, 1);
    m_gesturesBuffer.Resize(5, 1);
    m_gesturesBuffer.SetChannelNames({"pinch", "grab", "palm_norm_x", "palm_norm_y", "palm_norm_z"});
}

bool OpenXRHandTrackingChanOp::Cook(const CookContext& /*context*/) {
    auto& xr = OpenXRRuntime::Instance();
    const auto& rightHand = xr.GetHandTracking(XRHand::Right);

    // Gestures
    m_gesturesBuffer.Resize(5, 1);
    m_gesturesBuffer.SetChannelNames({"pinch", "grab", "palm_norm_x", "palm_norm_y", "palm_norm_z"});
    m_gesturesBuffer.GetChannelData(0)[0] = rightHand.pinchStrength;
    m_gesturesBuffer.GetChannelData(1)[0] = rightHand.grabStrength;
    m_gesturesBuffer.GetChannelData(2)[0] = rightHand.palmNormal.x;
    m_gesturesBuffer.GetChannelData(3)[0] = rightHand.palmNormal.y;
    m_gesturesBuffer.GetChannelData(4)[0] = rightHand.palmNormal.z;

    // Skeleton joints (26 joints XYZ)
    size_t jointCount = std::min(rightHand.joints.size(), size_t(26));
    m_skeletonBuffer.Resize(jointCount * 3, 1);
    for (size_t i = 0; i < jointCount; ++i) {
        m_skeletonBuffer.GetChannelData(i * 3 + 0)[0] = rightHand.joints[i].position.x;
        m_skeletonBuffer.GetChannelData(i * 3 + 1)[0] = rightHand.joints[i].position.y;
        m_skeletonBuffer.GetChannelData(i * 3 + 2)[0] = rightHand.joints[i].position.z;
    }

    m_outSkeletonPin->SetValue(PinValue(m_skeletonBuffer));
    m_outGesturesPin->SetValue(PinValue(m_gesturesBuffer));
    return true;
}

} // namespace nf
