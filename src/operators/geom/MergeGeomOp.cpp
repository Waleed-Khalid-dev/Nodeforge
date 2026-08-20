#include "MergeGeomOp.h"

namespace nf {

MergeGeomOp::MergeGeomOp(NodeId id, const std::string& name)
    : GeomOp(id, name, "MergeGeomOp") {
    m_inPin1 = AddInputPin("input1", PinType::Geom);
    m_inPin2 = AddInputPin("input2", PinType::Geom);
    m_outPin = AddOutputPin("output", PinType::Geom);

    SetParam("recompute_normals", false);
}

bool MergeGeomOp::Cook(const CookContext& /*context*/) {
    const GeometryData* g1 = GetInputGeometry(0);
    const GeometryData* g2 = GetInputGeometry(1);

    if (!g1 && !g2) {
        SetOutputGeometry(GeometryData{});
        return true;
    }
    if (!g1) {
        SetOutputGeometry(*g2);
        return true;
    }
    if (!g2) {
        SetOutputGeometry(*g1);
        return true;
    }

    GeometryData merged = *g1;
    merged.Merge(*g2);

    bool recompute = GetParam("recompute_normals").Is<bool>() ? GetParam("recompute_normals").Get<bool>() : false;
    if (recompute) {
        merged.ComputeNormals(true);
    }

    SetOutputGeometry(merged);
    return true;
}

} // namespace nf
