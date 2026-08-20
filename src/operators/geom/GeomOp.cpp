#include "GeomOp.h"

namespace nf {

GeomOp::GeomOp(NodeId id, const std::string& name, const std::string& typeName)
    : Node(id, name, typeName) {
}

const GeometryData* GeomOp::GetOutputGeometry() const {
    if (m_outPin && m_outPin->GetValue().Is<GeometryData>()) {
        return &m_outPin->GetValue().Get<GeometryData>();
    }
    return nullptr;
}

const GeometryData* GeomOp::GetInputGeometry(size_t index) const {
    if (index < m_inputPins.size()) {
        const auto& val = m_inputPins[index]->GetValue();
        if (val.Is<GeometryData>()) {
            return &val.Get<GeometryData>();
        }
    }
    return nullptr;
}

size_t GeomOp::GetVertexCount() const {
    const GeometryData* geom = GetOutputGeometry();
    return geom ? geom->GetVertexCount() : 0;
}

size_t GeomOp::GetTriangleCount() const {
    const GeometryData* geom = GetOutputGeometry();
    return geom ? geom->GetTriangleCount() : 0;
}

BoundingBox GeomOp::GetBounds() const {
    const GeometryData* geom = GetOutputGeometry();
    return geom ? geom->ComputeBounds() : BoundingBox{};
}

void GeomOp::SetOutputGeometry(const GeometryData& geom) {
    m_cachedGeometry = geom;
    if (m_outPin) {
        m_outPin->SetValue(PinValue(m_cachedGeometry));
    }
}

} // namespace nf
