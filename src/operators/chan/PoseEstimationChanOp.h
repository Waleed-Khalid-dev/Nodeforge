#pragma once

#include "../../graph/Node.h"
#include "../../core/ChannelBuffer.h"

namespace nf {

class PoseEstimationChanOp : public Node {
public:
    PoseEstimationChanOp(NodeId id, const std::string& name);
    ~PoseEstimationChanOp() override = default;

    bool Cook(const CookContext& context) override;

    float GetConfidenceThreshold() const;
    float GetSmoothing() const;

private:
    Pin* m_inTexPin = nullptr;
    Pin* m_outKeypointsPin = nullptr;
    Pin* m_outConfidencePin = nullptr;
    ChannelBuffer m_keypointsBuffer;
    ChannelBuffer m_confidenceBuffer;
};

} // namespace nf
