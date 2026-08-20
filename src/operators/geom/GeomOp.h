#pragma once

#include "../../graph/Node.h"
#include "../../core/GeometryData.h"

namespace nf {

class GeomOp : public Node {
public:
    GeomOp(NodeId id, const std::string& name, const std::string& typeName);
    virtual ~GeomOp() = default;

    const GeometryData* GetOutputGeometry() const;
    const GeometryData* GetInputGeometry(size_t index = 0) const;

    size_t GetVertexCount() const;
    size_t GetTriangleCount() const;
    BoundingBox GetBounds() const;

    void SetOutputGeometry(const GeometryData& geom);

protected:
    GeometryData m_cachedGeometry;
    Pin* m_outPin = nullptr;
};

} // namespace nf
