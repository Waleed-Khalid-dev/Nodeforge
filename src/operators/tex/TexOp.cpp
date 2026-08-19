#include "TexOp.h"

namespace nf {

TexOp::TexOp(NodeId id, const std::string& name, const std::string& typeName)
    : Node(id, name, typeName) {
}

std::shared_ptr<gpu::Texture2D> TexOp::GetInputTexture(size_t index) const {
    if (index >= m_inputPins.size()) return nullptr;
    const auto& inPin = m_inputPins[index];
    if (!inPin || !inPin->IsConnected()) return nullptr;

    const Pin* srcPin = inPin->GetConnectedSource();
    if (!srcPin) return nullptr;

    const PinValue& val = srcPin->GetValue();
    if (val.Is<std::shared_ptr<gpu::Texture2D>>()) {
        return val.Get<std::shared_ptr<gpu::Texture2D>>();
    }
    return nullptr;
}

void TexOp::SetOutputTexture(std::shared_ptr<gpu::Texture2D> texture) {
    m_outputTexture = texture;
    if (m_outPin) {
        m_outPin->SetValue(PinValue(texture));
    }
}

} // namespace nf
