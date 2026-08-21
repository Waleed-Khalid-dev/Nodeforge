#pragma once

#include "../../graph/Node.h"
#include "../../core/ChannelBuffer.h"

namespace nf {

class OpenXRHandTrackingChanOp : public Node {
public:
    OpenXRHandTrackingChanOp(NodeId id, const std::string& name);
    ~OpenXRHandTrackingChanOp() override = default;

    bool Cook(const CookContext& context) override;

private:
    Pin* m_outSkeletonPin = nullptr;
    Pin* m_outGesturesPin = nullptr;
    ChannelBuffer m_skeletonBuffer;
    ChannelBuffer m_gesturesBuffer;
};

} // namespace nf
