#include "NeuralStyleTexOp.h"
#include "../../ai/ONNXInferenceEngine.h"

namespace nf {

NeuralStyleTexOp::NeuralStyleTexOp(NodeId id, const std::string& name)
    : Node(id, name, "NeuralStyleTexOp") {
    m_inTexPin = AddInputPin("input", PinType::Tex);
    m_outTexPin = AddOutputPin("output", PinType::Tex);

    SetParam("style_preset", static_cast<int32_t>(4)); // 0: Candy, 1: Mosaic, 2: Rain Princess, 3: Udnie, 4: Cyberpunk Neon, 5: Ink Wash
    SetParam("style_weight", 1.0f);
    SetParam("preserve_colors", false);
}

int32_t NeuralStyleTexOp::GetStylePreset() const {
    return GetParam("style_preset").Is<int32_t>() ? GetParam("style_preset").Get<int32_t>() : 4;
}

float NeuralStyleTexOp::GetStyleWeight() const {
    return GetParam("style_weight").Is<float>() ? GetParam("style_weight").Get<float>() : 1.0f;
}

bool NeuralStyleTexOp::GetPreserveColors() const {
    return GetParam("preserve_colors").Is<bool>() ? GetParam("preserve_colors").Get<bool>() : false;
}

bool NeuralStyleTexOp::Cook(const CookContext& /*context*/) {
    if (m_inTexPin && m_inTexPin->GetValue().Is<std::shared_ptr<gpu::Texture2D>>()) {
        m_outTexture = m_inTexPin->GetValue().Get<std::shared_ptr<gpu::Texture2D>>();
    }

    if (m_outTexture) {
        m_outTexPin->SetValue(PinValue(m_outTexture));
    }
    return true;
}

} // namespace nf
