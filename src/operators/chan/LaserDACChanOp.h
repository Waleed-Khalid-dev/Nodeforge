#pragma once

#include "../../graph/Node.h"
#include "../../core/ChannelBuffer.h"

namespace nf {

class LaserDACChanOp : public Node {
public:
    LaserDACChanOp(NodeId id, const std::string& name);
    ~LaserDACChanOp() override = default;

    bool Cook(const CookContext& context) override;

    int32_t GetDACType() const;
    std::string GetIPAddress() const;
    bool IsSafetyShutterOpen() const;
    float GetMasterBrightness() const;

private:
    Pin* m_inPointsPin = nullptr;
    Pin* m_outTelemetryPin = nullptr;
    ChannelBuffer m_telemetryBuffer;
};

} // namespace nf
