#include "BoxGeomOp.h"

namespace nf {

BoxGeomOp::BoxGeomOp(NodeId id, const std::string& name)
    : GeomOp(id, name, "BoxGeomOp") {
    m_outPin = AddOutputPin("output", PinType::Geom);

    SetParam("size_x", 1.0f);
    SetParam("size_y", 1.0f);
    SetParam("size_z", 1.0f);
    SetParam("divs_x", static_cast<int32_t>(1));
    SetParam("divs_y", static_cast<int32_t>(1));
    SetParam("divs_z", static_cast<int32_t>(1));
}

bool BoxGeomOp::Cook(const CookContext& /*context*/) {
    float sx = GetParam("size_x").Is<float>() ? GetParam("size_x").Get<float>() : 1.0f;
    float sy = GetParam("size_y").Is<float>() ? GetParam("size_y").Get<float>() : 1.0f;
    float sz = GetParam("size_z").Is<float>() ? GetParam("size_z").Get<float>() : 1.0f;
    int32_t dx = GetParam("divs_x").Is<int32_t>() ? GetParam("divs_x").Get<int32_t>() : 1;
    int32_t dy = GetParam("divs_y").Is<int32_t>() ? GetParam("divs_y").Get<int32_t>() : 1;
    int32_t dz = GetParam("divs_z").Is<int32_t>() ? GetParam("divs_z").Get<int32_t>() : 1;

    GeometryData box = GeometryData::CreateBox(sx, sy, sz, dx, dy, dz);
    SetOutputGeometry(box);
    return true;
}

} // namespace nf
