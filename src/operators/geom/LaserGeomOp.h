#pragma once

#include "../../graph/Node.h"
#include "../../core/GeometryData.h"
#include "../../core/ChannelBuffer.h"

namespace nf {

class LaserGeomOp : public Node {
public:
    LaserGeomOp(NodeId id, const std::string& name);
    ~LaserGeomOp() override = default;

    bool Cook(const CookContext& context) override;

    int32_t GetTargetPPS() const;
    int32_t GetBlankDelay() const;
    int32_t GetCornerDwell() const;
    glm::vec4 GetColorOverride() const;

private:
    Pin* m_inGeomPin = nullptr;
    Pin* m_outGeomPin = nullptr;
    Pin* m_outPointsPin = nullptr;
    GeometryData m_outGeom;
    ChannelBuffer m_pointsBuffer;
};

} // namespace nf
