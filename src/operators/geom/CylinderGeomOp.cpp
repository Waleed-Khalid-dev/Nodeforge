#include "CylinderGeomOp.h"

namespace nf {

CylinderGeomOp::CylinderGeomOp(NodeId id, const std::string& name)
    : GeomOp(id, name, "CylinderGeomOp") {
    m_outPin = AddOutputPin("output", PinType::Geom);

    SetParam("height", 2.0f);
    SetParam("radius_bottom", 1.0f);
    SetParam("radius_top", 1.0f);
    SetParam("segments", static_cast<int32_t>(32));
    SetParam("cap_bottom", true);
    SetParam("cap_top", true);
}

bool CylinderGeomOp::Cook(const CookContext& /*context*/) {
    float height = GetParam("height").Is<float>() ? GetParam("height").Get<float>() : 2.0f;
    float rBottom = GetParam("radius_bottom").Is<float>() ? GetParam("radius_bottom").Get<float>() : 1.0f;
    float rTop = GetParam("radius_top").Is<float>() ? GetParam("radius_top").Get<float>() : 1.0f;
    int32_t segments = GetParam("segments").Is<int32_t>() ? GetParam("segments").Get<int32_t>() : 32;
    bool capBottom = GetParam("cap_bottom").Is<bool>() ? GetParam("cap_bottom").Get<bool>() : true;
    bool capTop = GetParam("cap_top").Is<bool>() ? GetParam("cap_top").Get<bool>() : true;

    GeometryData cyl = GeometryData::CreateCylinder(height, rBottom, rTop, segments, capBottom, capTop);
    SetOutputGeometry(cyl);
    return true;
}

} // namespace nf
