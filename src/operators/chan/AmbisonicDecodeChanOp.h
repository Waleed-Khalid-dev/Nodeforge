#pragma once

#include "../../graph/Node.h"
#include "../../core/ChannelBuffer.h"

namespace nf {

class AmbisonicDecodeChanOp : public Node {
public:
    AmbisonicDecodeChanOp(NodeId id, const std::string& name);
    ~AmbisonicDecodeChanOp() override = default;

    bool Cook(const CookContext& context) override;

private:
    Pin* m_inBFormatPin = nullptr;
    Pin* m_inLayoutDataPin = nullptr;
    Pin* m_outSpeakersPin = nullptr;
    ChannelBuffer m_outBuffer;
};

} // namespace nf
