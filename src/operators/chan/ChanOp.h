#pragma once

#include "../../graph/Node.h"
#include "../../core/ChannelBuffer.h"

namespace nf {

/// Base class for all Channel Operators (ChanOps).
class ChanOp : public Node {
public:
    ChanOp(NodeId id, const std::string& name, const std::string& typeName);
    virtual ~ChanOp() = default;

    const ChannelBuffer* GetOutputBuffer() const;
    const ChannelBuffer* GetInputBuffer(size_t index = 0) const;

    float GetSample(const std::string& channelName, size_t sampleIdx = 0) const;

protected:
    void SetOutputBuffer(const ChannelBuffer& buffer);

    Pin* m_outPin = nullptr;
    ChannelBuffer m_cachedBuffer;
};

} // namespace nf
