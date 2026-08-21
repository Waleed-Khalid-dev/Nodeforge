#include "OpenXRCameraComp.h"
#include "../../xr/OpenXRRuntime.h"

namespace nf {

OpenXRCameraComp::OpenXRCameraComp(NodeId id, const std::string& name)
    : Node(id, name, "OpenXRCameraComp") {
    m_outCompPin = AddOutputPin("output", PinType::Any);
    m_outHeadPosePin = AddOutputPin("out_head_pose", PinType::Chan);

    SetParam("tracking_space", static_cast<int32_t>(1)); // 0: Local Seated, 1: Stage Room-Scale
    SetParam("camera_offset", glm::vec3(0.0f, 0.0f, 0.0f));

    m_headPoseBuffer.Resize(7, 1);
    m_headPoseBuffer.SetChannelNames({"tx", "ty", "tz", "rx", "ry", "rz", "rw"});
}

glm::vec3 OpenXRCameraComp::GetCameraOffset() const {
    return GetParam("camera_offset").Is<glm::vec3>() ? GetParam("camera_offset").Get<glm::vec3>() : glm::vec3(0.0f);
}

int32_t OpenXRCameraComp::GetTrackingSpace() const {
    return GetParam("tracking_space").Is<int32_t>() ? GetParam("tracking_space").Get<int32_t>() : 1;
}

bool OpenXRCameraComp::Cook(const CookContext& /*context*/) {
    auto& xr = OpenXRRuntime::Instance();
    glm::vec3 headPos = xr.GetHeadPosition() + GetCameraOffset();
    glm::quat headRot = xr.GetHeadOrientation();

    m_headPoseBuffer.Resize(7, 1);
    m_headPoseBuffer.SetChannelNames({"tx", "ty", "tz", "rx", "ry", "rz", "rw"});
    m_headPoseBuffer.GetChannelData(0)[0] = headPos.x;
    m_headPoseBuffer.GetChannelData(1)[0] = headPos.y;
    m_headPoseBuffer.GetChannelData(2)[0] = headPos.z;
    m_headPoseBuffer.GetChannelData(3)[0] = headRot.x;
    m_headPoseBuffer.GetChannelData(4)[0] = headRot.y;
    m_headPoseBuffer.GetChannelData(5)[0] = headRot.z;
    m_headPoseBuffer.GetChannelData(6)[0] = headRot.w;

    m_outHeadPosePin->SetValue(PinValue(m_headPoseBuffer));
    return true;
}

} // namespace nf
