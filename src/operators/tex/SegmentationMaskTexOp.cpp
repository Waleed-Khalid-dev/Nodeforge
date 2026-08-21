#include "SegmentationMaskTexOp.h"
#include "../../ai/ONNXInferenceEngine.h"

namespace nf {

SegmentationMaskTexOp::SegmentationMaskTexOp(NodeId id, const std::string& name)
    : Node(id, name, "SegmentationMaskTexOp") {
    m_inTexPin = AddInputPin("input", PinType::Tex);
    m_outTexPin = AddOutputPin("output", PinType::Tex);
    m_outMaskPin = AddOutputPin("out_mask", PinType::Tex);

    SetParam("threshold", 0.5f);
    SetParam("feather", 2.0f);
}

float SegmentationMaskTexOp::GetThreshold() const {
    return GetParam("threshold").Is<float>() ? GetParam("threshold").Get<float>() : 0.5f;
}

float SegmentationMaskTexOp::GetFeather() const {
    return GetParam("feather").Is<float>() ? GetParam("feather").Get<float>() : 2.0f;
}

bool SegmentationMaskTexOp::Cook(const CookContext& /*context*/) {
    if (m_inTexPin && m_inTexPin->GetValue().Is<std::shared_ptr<gpu::Texture2D>>()) {
        m_outTexture = m_inTexPin->GetValue().Get<std::shared_ptr<gpu::Texture2D>>();
        m_outMask = m_outTexture;
    }

    if (m_outTexture) {
        m_outTexPin->SetValue(PinValue(m_outTexture));
        m_outMaskPin->SetValue(PinValue(m_outMask));
    }
    return true;
}

} // namespace nf
