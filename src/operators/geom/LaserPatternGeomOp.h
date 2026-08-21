#pragma once

#include "../../graph/Node.h"
#include "../../core/GeometryData.h"

namespace nf {

class LaserPatternGeomOp : public Node {
public:
    LaserPatternGeomOp(NodeId id, const std::string& name);
    ~LaserPatternGeomOp() override = default;

    bool Cook(const CookContext& context) override;

    int32_t GetPatternType() const;
    float GetFrequencyA() const;
    float GetFrequencyB() const;
    float GetPhase() const;
    int32_t GetBeamCount() const;

private:
    Pin* m_outGeomPin = nullptr;
    GeometryData m_geomData;
};

} // namespace nf
