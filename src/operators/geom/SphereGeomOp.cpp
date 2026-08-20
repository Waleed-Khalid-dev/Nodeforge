#include "SphereGeomOp.h"

namespace nf {

SphereGeomOp::SphereGeomOp(NodeId id, const std::string& name)
    : GeomOp(id, name, "SphereGeomOp") {
    m_outPin = AddOutputPin("output", PinType::Geom);

    SetParam("radius", 1.0f);
    SetParam("rings", static_cast<int32_t>(16));
    SetParam("segments", static_cast<int32_t>(32));
}

bool SphereGeomOp::Cook(const CookContext& /*context*/) {
    float radius = GetParam("radius").Is<float>() ? GetParam("radius").Get<float>() : 1.0f;
    int32_t rings = GetParam("rings").Is<int32_t>() ? GetParam("rings").Get<int32_t>() : 16;
    int32_t segments = GetParam("segments").Is<int32_t>() ? GetParam("segments").Get<int32_t>() : 32;

    GeometryData sphere = GeometryData::CreateSphere(radius, rings, segments);
    SetOutputGeometry(sphere);
    return true;
}

} // namespace nf
