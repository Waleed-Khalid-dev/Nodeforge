#include "TransformTexOp.h"

namespace nf {

TransformTexOp::TransformTexOp(NodeId id, const std::string& name)
    : Node(id, name, "TransformTexOp") {
    m_inPin = AddInputPin("input", PinType::Tex);
    m_outPin = AddOutputPin("output", PinType::Tex);

    SetParam("translate", glm::vec2(0.0f, 0.0f));
    SetParam("rotate", 0.0f);
    SetParam("scale", glm::vec2(1.0f, 1.0f));
    SetParam("pivot", glm::vec2(0.5f, 0.5f));
}

bool TransformTexOp::Cook(const CookContext& /*context*/) {
    const PinValue& inVal = m_inPin->GetValue();
    if (inVal.Is<std::shared_ptr<gpu::Texture2D>>()) {
        m_outPin->SetValue(inVal);
    } else {
        m_outPin->SetValue(PinValue(std::shared_ptr<gpu::Texture2D>(nullptr)));
    }
    return true;
}

} // namespace nf
