#include "NormalsGeomOp.h"

namespace nf {

NormalsGeomOp::NormalsGeomOp(NodeId id, const std::string& name)
    : GeomOp(id, name, "NormalsGeomOp") {
    m_inPin = AddInputPin("input", PinType::Geom);
    m_outPin = AddOutputPin("output", PinType::Geom);

    SetParam("mode", static_cast<int32_t>(0)); // 0: Smooth, 1: Faceted/Flat, 2: Invert
    SetParam("compute_tangents", true);
}

bool NormalsGeomOp::Cook(const CookContext& /*context*/) {
    const GeometryData* inGeom = GetInputGeometry(0);
    if (!inGeom || inGeom->IsEmpty()) {
        SetOutputGeometry(GeometryData{});
        return true;
    }

    int32_t mode = GetParam("mode").Is<int32_t>() ? GetParam("mode").Get<int32_t>() : 0;
    bool calcTangents = GetParam("compute_tangents").Is<bool>() ? GetParam("compute_tangents").Get<bool>() : true;

    GeometryData outGeom = *inGeom;
    if (mode == 0) {
        outGeom.ComputeNormals(true);
    } else if (mode == 1) {
        outGeom.ComputeNormals(false);
    } else if (mode == 2) {
        for (auto& v : outGeom.GetVertices()) {
            v.normal = -v.normal;
        }
    }

    if (calcTangents) {
        outGeom.ComputeTangents();
    }

    SetOutputGeometry(outGeom);
    return true;
}

} // namespace nf
