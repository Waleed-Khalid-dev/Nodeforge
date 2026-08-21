#pragma once

#include "../../graph/Node.h"
#include "../../core/GeometryData.h"

namespace nf {

class VoxelGridGeomOp : public Node {
public:
    VoxelGridGeomOp(NodeId id, const std::string& name);
    ~VoxelGridGeomOp() override = default;

    bool Cook(const CookContext& context) override;

    int32_t GetGridResolution() const;
    glm::vec3 GetBoxSize() const;
    int32_t GetFieldType() const;

private:
    Pin* m_outGeomPin = nullptr;
    GeometryData m_geomData;
};

} // namespace nf
