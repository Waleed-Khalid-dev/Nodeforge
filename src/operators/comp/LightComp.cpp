#include "LightComp.h"
#include <glm/gtc/matrix_transform.hpp>

namespace nf {

LightComp::LightComp(NodeId id, const std::string& name)
    : Node(id, name, "LightComp") {
    m_outPin = AddOutputPin("output", PinType::Any);

    SetParam("light_type", static_cast<int32_t>(0)); // 0: Directional, 1: Point, 2: Ambient
    SetParam("color", glm::vec3(1.0f, 1.0f, 1.0f));
    SetParam("intensity", 1.0f);
    SetParam("position", glm::vec3(5.0f, 5.0f, 5.0f));
    SetParam("direction", glm::vec3(-0.577f, -0.577f, -0.577f));
    SetParam("attenuation", 0.05f);
}

bool LightComp::Cook(const CookContext& /*context*/) {
    return true;
}

LightData LightComp::GetLightData() const {
    LightData data{};
    int32_t ltype = GetParam("light_type").Is<int32_t>() ? GetParam("light_type").Get<int32_t>() : 0;
    data.type = static_cast<LightType>(ltype);
    data.color = GetParam("color").Is<glm::vec3>() ? GetParam("color").Get<glm::vec3>() : glm::vec3(1.0f);
    data.intensity = GetParam("intensity").Is<float>() ? GetParam("intensity").Get<float>() : 1.0f;
    data.position = GetParam("position").Is<glm::vec3>() ? GetParam("position").Get<glm::vec3>() : glm::vec3(5.0f);
    glm::vec3 dir = GetParam("direction").Is<glm::vec3>() ? GetParam("direction").Get<glm::vec3>() : glm::vec3(-0.577f);
    data.direction = glm::length(dir) > 1e-6f ? glm::normalize(dir) : glm::vec3(0, -1, 0);
    data.attenuation = GetParam("attenuation").Is<float>() ? GetParam("attenuation").Get<float>() : 0.05f;
    return data;
}

} // namespace nf
