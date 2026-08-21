#pragma once

#include "../../graph/Node.h"
#include "../../core/ChannelBuffer.h"

namespace nf {

class OpenXRControllerChanOp : public Node {
public:
    OpenXRControllerChanOp(NodeId id, const std::string& name);
    ~OpenXRControllerChanOp() override = default;

    bool Cook(const CookContext& context) override;

private:
    Pin* m_outLeftPin = nullptr;
    Pin* m_outRightPin = nullptr;
    ChannelBuffer m_leftBuffer;
    ChannelBuffer m_rightBuffer;
};

} // namespace nf
