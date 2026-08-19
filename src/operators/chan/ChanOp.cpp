#include "ChanOp.h"

namespace nf {

ChanOp::ChanOp(NodeId id, const std::string& name, const std::string& typeName)
    : Node(id, name, typeName) {
}

const ChannelBuffer* ChanOp::GetOutputBuffer() const {
    if (m_outPin && m_outPin->GetValue().Is<ChannelBuffer>()) {
        return &m_outPin->GetValue().Get<ChannelBuffer>();
    }
    return nullptr;
}

const ChannelBuffer* ChanOp::GetInputBuffer(size_t index) const {
    if (index < m_inputPins.size()) {
        const auto& val = m_inputPins[index]->GetValue();
        if (val.Is<ChannelBuffer>()) {
            return &val.Get<ChannelBuffer>();
        }
    }
    return nullptr;
}

float ChanOp::GetSample(const std::string& channelName, size_t sampleIdx) const {
    const ChannelBuffer* buf = GetOutputBuffer();
    if (buf) {
        return buf->GetSample(channelName, sampleIdx);
    }
    return 0.0f;
}

void ChanOp::SetOutputBuffer(const ChannelBuffer& buffer) {
    m_cachedBuffer = buffer;
    if (m_outPin) {
        m_outPin->SetValue(PinValue(m_cachedBuffer));
    }
}

} // namespace nf
