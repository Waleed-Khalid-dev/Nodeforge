#pragma once

#include "../../graph/Node.h"
#include <glm/glm.hpp>

namespace nf {

enum class LightType {
    Directional,
    Point,
    Ambient
};

struct LightData {
    LightType type = LightType::Directional;
    glm::vec3 color{1.0f};
    float intensity = 1.0f;
    glm::vec3 position{5.0f, 5.0f, 5.0f};
    glm::vec3 direction{-0.577f, -0.577f, -0.577f};
    float attenuation = 0.05f;
};

class LightComp : public Node {
public:
    LightComp(NodeId id, const std::string& name);

    bool Cook(const CookContext& context) override;

    LightData GetLightData() const;

private:
    Pin* m_outPin = nullptr;
};

} // namespace nf
