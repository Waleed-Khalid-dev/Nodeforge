#include "VolumetricCloudTexOp.h"
#include "../../render/VolumetricMath.h"

namespace nf {

VolumetricCloudTexOp::VolumetricCloudTexOp(NodeId id, const std::string& name)
    : Node(id, name, "VolumetricCloudTexOp") {
    m_inEnvPin = AddInputPin("in_env", PinType::Tex);
    m_outTexPin = AddOutputPin("output", PinType::Tex);

    SetParam("density", 1.0f);
    SetParam("sun_dir", glm::vec3(0.5f, 0.8f, -0.3f));
    SetParam("sun_color", glm::vec4(1.0f, 0.9f, 0.7f, 1.0f));
    SetParam("scattering_g", 0.6f);
    SetParam("step_count", static_cast<int32_t>(32));
    SetParam("wind_speed", 0.5f);
}

float VolumetricCloudTexOp::GetDensity() const {
    return GetParam("density").Is<float>() ? GetParam("density").Get<float>() : 1.0f;
}

glm::vec3 VolumetricCloudTexOp::GetSunDirection() const {
    return GetParam("sun_dir").Is<glm::vec3>() ? GetParam("sun_dir").Get<glm::vec3>() : glm::vec3(0.5f, 0.8f, -0.3f);
}

glm::vec4 VolumetricCloudTexOp::GetSunColor() const {
    return GetParam("sun_color").Is<glm::vec4>() ? GetParam("sun_color").Get<glm::vec4>() : glm::vec4(1.0f, 0.9f, 0.7f, 1.0f);
}

float VolumetricCloudTexOp::GetScatteringG() const {
    return GetParam("scattering_g").Is<float>() ? GetParam("scattering_g").Get<float>() : 0.6f;
}

int32_t VolumetricCloudTexOp::GetStepCount() const {
    return GetParam("step_count").Is<int32_t>() ? GetParam("step_count").Get<int32_t>() : 32;
}

float VolumetricCloudTexOp::GetWindSpeed() const {
    return GetParam("wind_speed").Is<float>() ? GetParam("wind_speed").Get<float>() : 0.5f;
}

bool VolumetricCloudTexOp::Cook(const CookContext& context) {
    if (m_inEnvPin && m_inEnvPin->GetValue().Is<std::shared_ptr<gpu::Texture2D>>()) {
        m_outTexture = m_inEnvPin->GetValue().Get<std::shared_ptr<gpu::Texture2D>>();
    }

    // In raster/headless cook, calculate sample cloud raymarch
    glm::vec3 origin(0.0f, 0.0f, 0.0f);
    glm::vec3 dir(0.0f, 0.5f, 1.0f);
    VolumetricMath::RayMarchClouds(
        origin,
        dir,
        GetSunDirection(),
        GetSunColor(),
        GetDensity(),
        GetScatteringG(),
        GetStepCount(),
        static_cast<float>(context.timeSeconds),
        GetWindSpeed()
    );

    if (m_outTexture) {
        m_outTexPin->SetValue(PinValue(m_outTexture));
    }
    return true;
}

} // namespace nf
