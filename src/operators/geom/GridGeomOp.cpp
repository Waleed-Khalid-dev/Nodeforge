#include "GridGeomOp.h"

namespace nf {

GridGeomOp::GridGeomOp(NodeId id, const std::string& name)
    : GeomOp(id, name, "GridGeomOp") {
    m_outPin = AddOutputPin("output", PinType::Geom);

    SetParam("size_x", 1.0f);
    SetParam("size_y", 1.0f);
    SetParam("rows", static_cast<int32_t>(10));
    SetParam("cols", static_cast<int32_t>(10));
    SetParam("plane", static_cast<int32_t>(0)); // 0: XY, 1: XZ, 2: YZ
}

bool GridGeomOp::Cook(const CookContext& /*context*/) {
    float sx = GetParam("size_x").Is<float>() ? GetParam("size_x").Get<float>() : 1.0f;
    float sy = GetParam("size_y").Is<float>() ? GetParam("size_y").Get<float>() : 1.0f;
    int32_t rows = GetParam("rows").Is<int32_t>() ? GetParam("rows").Get<int32_t>() : 10;
    int32_t cols = GetParam("cols").Is<int32_t>() ? GetParam("cols").Get<int32_t>() : 10;
    int32_t plane = GetParam("plane").Is<int32_t>() ? GetParam("plane").Get<int32_t>() : 0;

    GeometryData grid = GeometryData::CreateGrid(sx, sy, rows, cols, plane);
    SetOutputGeometry(grid);
    return true;
}

} // namespace nf
