#include "TorusGeomOp.h"

namespace nf {

TorusGeomOp::TorusGeomOp(NodeId id, const std::string& name)
    : GeomOp(id, name, "TorusGeomOp") {
    m_outPin = AddOutputPin("output", PinType::Geom);

    SetParam("major_radius", 1.0f);
    SetParam("minor_radius", 0.3f);
    SetParam("major_segments", static_cast<int32_t>(32));
    SetParam("minor_segments", static_cast<int32_t>(16));
}

bool TorusGeomOp::Cook(const CookContext& /*context*/) {
    float majorR = GetParam("major_radius").Is<float>() ? GetParam("major_radius").Get<float>() : 1.0f;
    float minorR = GetParam("minor_radius").Is<float>() ? GetParam("minor_radius").Get<float>() : 0.3f;
    int32_t majorSegs = GetParam("major_segments").Is<int32_t>() ? GetParam("major_segments").Get<int32_t>() : 32;
    int32_t minorSegs = GetParam("minor_segments").Is<int32_t>() ? GetParam("minor_segments").Get<int32_t>() : 16;

    GeometryData torus = GeometryData::CreateTorus(majorR, minorR, majorSegs, minorSegs);
    SetOutputGeometry(torus);
    return true;
}

} // namespace nf
