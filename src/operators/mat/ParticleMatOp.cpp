#include "ParticleMatOp.h"

namespace nf {

ParticleMatOp::ParticleMatOp(NodeId id, const std::string& name)
    : MatOp(id, name, "ParticleMatOp") {
    m_inTexturePin = AddInputPin("texture", PinType::Tex);
    m_outPin = AddOutputPin("output", PinType::Mat);

    SetParam("blend_mode", static_cast<int32_t>(0)); // 0: Additive, 1: Alpha Blend, 2: Screen
    SetParam("point_type", static_cast<int32_t>(0)); // 0: Point Sprite, 1: Billboard Quad
    SetParam("base_size", 0.1f);
    SetParam("size_attenuation", true);
    SetParam("soft_falloff", true);
    SetParam("depth_fade", 0.2f);
    SetParam("color_multiplier", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    m_materialData.type = MaterialType::Constant;
    m_materialData.useVertexColor = true;
}

bool ParticleMatOp::Cook(const CookContext& /*context*/) {
    m_materialData.color = GetParam("color_multiplier").Is<glm::vec4>() ? GetParam("color_multiplier").Get<glm::vec4>() : glm::vec4(1.0f);
    m_materialData.useVertexColor = true;

    return true;
}

} // namespace nf
