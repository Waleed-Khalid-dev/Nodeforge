#pragma once

#include "../../graph/Node.h"
#include "../../core/GeometryData.h"

namespace nf {

class LaserFileInGeomOp : public Node {
public:
    LaserFileInGeomOp(NodeId id, const std::string& name);
    ~LaserFileInGeomOp() override = default;

    bool Cook(const CookContext& context) override;

    std::string GetFilePath() const;
    int32_t GetPlayMode() const;
    float GetSpeed() const;

private:
    Pin* m_outGeomPin = nullptr;
    GeometryData m_geomData;
};

} // namespace nf
