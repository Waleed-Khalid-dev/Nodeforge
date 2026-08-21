#include "ColliderGeomOp.h"

namespace nf {

ColliderGeomOp::ColliderGeomOp(NodeId id, const std::string& name)
    : Node(id, name, "ColliderGeomOp") {
    m_inGeomPin = AddInputPin("in_geom", PinType::Geom);
    m_outGeomPin = AddOutputPin("output", PinType::Geom);

    SetParam("shape_type", static_cast<int32_t>(0)); // 0: Ground Plane, 1: Box Bounds, 2: Sphere Bounds
    SetParam("size", glm::vec3(20.0f, 1.0f, 20.0f));
    SetParam("position", glm::vec3(0.0f, 0.0f, 0.0f));
    SetParam("restitution", 0.5f);
    SetParam("friction", 0.5f);
}

glm::vec3 ColliderGeomOp::GetSize() const {
    return GetParam("size").Is<glm::vec3>() ? GetParam("size").Get<glm::vec3>() : glm::vec3(20.0f, 1.0f, 20.0f);
}

glm::vec3 ColliderGeomOp::GetPosition() const {
    return GetParam("position").Is<glm::vec3>() ? GetParam("position").Get<glm::vec3>() : glm::vec3(0.0f);
}

float ColliderGeomOp::GetRestitution() const {
    return GetParam("restitution").Is<float>() ? GetParam("restitution").Get<float>() : 0.5f;
}

float ColliderGeomOp::GetFriction() const {
    return GetParam("friction").Is<float>() ? GetParam("friction").Get<float>() : 0.5f;
}

bool ColliderGeomOp::Cook(const CookContext& /*context*/) {
    int32_t shape = GetParam("shape_type").Is<int32_t>() ? GetParam("shape_type").Get<int32_t>() : 0;
    glm::vec3 sz = GetSize();

    if (shape == 0) {
        // Ground Grid Plane
        m_outGeometry = GeometryData::CreateGrid(sz.x, sz.z, 10, 10);
    } else if (shape == 1) {
        // Box Bounds
        m_outGeometry = GeometryData::CreateBox(sz.x, sz.y, sz.z);
    } else {
        // Sphere Bounds
        m_outGeometry = GeometryData::CreateSphere(sz.x * 0.5f, 16, 16);
    }

    m_outGeomPin->SetValue(PinValue(m_outGeometry));
    return true;
}

} // namespace nf
