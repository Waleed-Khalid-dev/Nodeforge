#include "VolumetricFogMatOp.h"

namespace nf {

VolumetricFogMatOp::VolumetricFogMatOp(NodeId id, const std::string& name)
    : Node(id, name, "VolumetricFogMatOp") {
    m_outMatPin = AddOutputPin("output", PinType::Any);

    SetParam("density_scale", 2.0f);
    SetParam("fog_color", glm::vec4(0.7f, 0.8f, 0.9f, 1.0f));
    SetParam("noise_frequency", 1.5f);
    SetParam("absorption", 0.5f);
}

float VolumetricFogMatOp::GetDensityScale() const {
    return GetParam("density_scale").Is<float>() ? GetParam("density_scale").Get<float>() : 2.0f;
}

glm::vec4 VolumetricFogMatOp::GetFogColor() const {
    return GetParam("fog_color").Is<glm::vec4>() ? GetParam("fog_color").Get<glm::vec4>() : glm::vec4(0.7f, 0.8f, 0.9f, 1.0f);
}

float VolumetricFogMatOp::GetNoiseFrequency() const {
    return GetParam("noise_frequency").Is<float>() ? GetParam("noise_frequency").Get<float>() : 1.5f;
}

float VolumetricFogMatOp::GetAbsorption() const {
    return GetParam("absorption").Is<float>() ? GetParam("absorption").Get<float>() : 0.5f;
}

bool VolumetricFogMatOp::Cook(const CookContext& /*context*/) {
    // Outputs material descriptor for Scene3DPass
    return true;
}

} // namespace nf
