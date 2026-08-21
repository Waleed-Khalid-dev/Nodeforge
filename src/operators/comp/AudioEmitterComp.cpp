#include "AudioEmitterComp.h"
#include "../../core/GeometryData.h"

namespace nf {

AudioEmitterComp::AudioEmitterComp(NodeId id, const std::string& name)
    : Node(id, name, "AudioEmitterComp") {
    m_inAudioPin = AddInputPin("in_audio", PinType::Chan);
    m_inGeomPin = AddInputPin("in_geom", PinType::Geom);
    m_outCompPin = AddOutputPin("output", PinType::Any);
    m_outAudioPin = AddOutputPin("out_audio", PinType::Chan);

    SetParam("translate", glm::vec3(0.0f, 0.0f, 0.0f));
    SetParam("radius", 1.0f);
    SetParam("cone_inner_angle", 360.0f);
    SetParam("cone_outer_gain", 0.0f);
}

glm::vec3 AudioEmitterComp::GetPosition() const {
    return GetParam("translate").Is<glm::vec3>() ? GetParam("translate").Get<glm::vec3>() : glm::vec3(0.0f);
}

float AudioEmitterComp::GetRadius() const {
    return GetParam("radius").Is<float>() ? GetParam("radius").Get<float>() : 1.0f;
}

bool AudioEmitterComp::Cook(const CookContext& /*context*/) {
    // If input audio connected, forward it
    if (m_inAudioPin && m_inAudioPin->GetValue().Is<ChannelBuffer>()) {
        m_outAudioPin->SetValue(m_inAudioPin->GetValue());
    }
    return true;
}

} // namespace nf
