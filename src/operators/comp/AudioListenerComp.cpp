#include "AudioListenerComp.h"

namespace nf {

AudioListenerComp::AudioListenerComp(NodeId id, const std::string& name)
    : Node(id, name, "AudioListenerComp") {
    m_inCamPin = AddInputPin("in_cam", PinType::Any);
    m_outCompPin = AddOutputPin("output", PinType::Any);
    m_outStatePin = AddOutputPin("out_state", PinType::Chan);

    SetParam("position", glm::vec3(0.0f, 0.0f, 0.0f));
    SetParam("orientation", glm::vec3(0.0f, 0.0f, 0.0f));

    m_outBuffer.Resize(5, 1);
    m_outBuffer.SetChannelNames({"pos_x", "pos_y", "pos_z", "yaw", "pitch"});
}

glm::vec3 AudioListenerComp::GetPosition() const {
    return GetParam("position").Is<glm::vec3>() ? GetParam("position").Get<glm::vec3>() : glm::vec3(0.0f);
}

glm::vec3 AudioListenerComp::GetOrientation() const {
    return GetParam("orientation").Is<glm::vec3>() ? GetParam("orientation").Get<glm::vec3>() : glm::vec3(0.0f);
}

bool AudioListenerComp::Cook(const CookContext& /*context*/) {
    glm::vec3 pos = GetPosition();
    glm::vec3 rot = GetOrientation();

    m_outBuffer.Resize(5, 1);
    m_outBuffer.SetChannelNames({"pos_x", "pos_y", "pos_z", "yaw", "pitch"});
    m_outBuffer.GetChannelData(0)[0] = pos.x;
    m_outBuffer.GetChannelData(1)[0] = pos.y;
    m_outBuffer.GetChannelData(2)[0] = pos.z;
    m_outBuffer.GetChannelData(3)[0] = rot.y; // yaw
    m_outBuffer.GetChannelData(4)[0] = rot.x; // pitch

    m_outStatePin->SetValue(PinValue(m_outBuffer));
    return true;
}

} // namespace nf
