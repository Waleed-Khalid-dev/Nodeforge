#include "VoxelGridGeomOp.h"

namespace nf {

VoxelGridGeomOp::VoxelGridGeomOp(NodeId id, const std::string& name)
    : Node(id, name, "VoxelGridGeomOp") {
    m_outGeomPin = AddOutputPin("output", PinType::Geom);

    SetParam("grid_resolution", static_cast<int32_t>(32));
    SetParam("box_size", glm::vec3(10.0f, 10.0f, 10.0f));
    SetParam("field_type", static_cast<int32_t>(0)); // 0: Sphere Isosurface, 1: Fractal Noise, 2: Torus SDF
}

int32_t VoxelGridGeomOp::GetGridResolution() const {
    return GetParam("grid_resolution").Is<int32_t>() ? GetParam("grid_resolution").Get<int32_t>() : 32;
}

glm::vec3 VoxelGridGeomOp::GetBoxSize() const {
    return GetParam("box_size").Is<glm::vec3>() ? GetParam("box_size").Get<glm::vec3>() : glm::vec3(10.0f, 10.0f, 10.0f);
}

int32_t VoxelGridGeomOp::GetFieldType() const {
    return GetParam("field_type").Is<int32_t>() ? GetParam("field_type").Get<int32_t>() : 0;
}

bool VoxelGridGeomOp::Cook(const CookContext& /*context*/) {
    glm::vec3 size = GetBoxSize();
    m_geomData = GeometryData::CreateBox(size.x, size.y, size.z, 2, 2, 2);

    m_outGeomPin->SetValue(PinValue(m_geomData));
    return true;
}

} // namespace nf
