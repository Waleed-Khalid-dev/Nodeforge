#include "LightShaftTexOp.h"

namespace nf {

LightShaftTexOp::LightShaftTexOp(NodeId id, const std::string& name)
    : Node(id, name, "LightShaftTexOp") {
    m_inTexPin = AddInputPin("input", PinType::Tex);
    m_inOcclusionPin = AddInputPin("in_occlusion", PinType::Tex);
    m_outTexPin = AddOutputPin("output", PinType::Tex);

    SetParam("light_pos_screen", glm::vec2(0.5f, 0.5f));
    SetParam("density", 0.8f);
    SetParam("decay", 0.95f);
    SetParam("weight", 0.5f);
    SetParam("samples", static_cast<int32_t>(32));
}

glm::vec2 LightShaftTexOp::GetLightPosScreen() const {
    return GetParam("light_pos_screen").Is<glm::vec2>() ? GetParam("light_pos_screen").Get<glm::vec2>() : glm::vec2(0.5f, 0.5f);
}

float LightShaftTexOp::GetDensity() const {
    return GetParam("density").Is<float>() ? GetParam("density").Get<float>() : 0.8f;
}

float LightShaftTexOp::GetDecay() const {
    return GetParam("decay").Is<float>() ? GetParam("decay").Get<float>() : 0.95f;
}

float LightShaftTexOp::GetWeight() const {
    return GetParam("weight").Is<float>() ? GetParam("weight").Get<float>() : 0.5f;
}

int32_t LightShaftTexOp::GetSamples() const {
    return GetParam("samples").Is<int32_t>() ? GetParam("samples").Get<int32_t>() : 32;
}

bool LightShaftTexOp::Cook(const CookContext& /*context*/) {
    if (m_inTexPin && m_inTexPin->GetValue().Is<std::shared_ptr<gpu::Texture2D>>()) {
        m_outTexture = m_inTexPin->GetValue().Get<std::shared_ptr<gpu::Texture2D>>();
        m_outTexPin->SetValue(PinValue(m_outTexture));
    }
    return true;
}

} // namespace nf
