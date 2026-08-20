#include "TransformGeomOp.h"
#include <glm/gtc/matrix_transform.hpp>

namespace nf {

TransformGeomOp::TransformGeomOp(NodeId id, const std::string& name)
    : GeomOp(id, name, "TransformGeomOp") {
    m_inPin = AddInputPin("input", PinType::Geom);
    m_outPin = AddOutputPin("output", PinType::Geom);

    SetParam("translate", glm::vec3(0.0f));
    SetParam("rotate", glm::vec3(0.0f));
    SetParam("scale", glm::vec3(1.0f));
    SetParam("pivot", glm::vec3(0.0f));
}

bool TransformGeomOp::Cook(const CookContext& /*context*/) {
    const GeometryData* inGeom = GetInputGeometry(0);
    if (!inGeom || inGeom->IsEmpty()) {
        SetOutputGeometry(GeometryData{});
        return true;
    }

    glm::vec3 t = GetParam("translate").Is<glm::vec3>() ? GetParam("translate").Get<glm::vec3>() : glm::vec3(0.0f);
    glm::vec3 r = GetParam("rotate").Is<glm::vec3>() ? GetParam("rotate").Get<glm::vec3>() : glm::vec3(0.0f);
    glm::vec3 s = GetParam("scale").Is<glm::vec3>() ? GetParam("scale").Get<glm::vec3>() : glm::vec3(1.0f);
    glm::vec3 p = GetParam("pivot").Is<glm::vec3>() ? GetParam("pivot").Get<glm::vec3>() : glm::vec3(0.0f);

    glm::mat4 mat(1.0f);
    mat = glm::translate(mat, t + p);
    mat = glm::rotate(mat, glm::radians(r.z), glm::vec3(0, 0, 1));
    mat = glm::rotate(mat, glm::radians(r.y), glm::vec3(0, 1, 0));
    mat = glm::rotate(mat, glm::radians(r.x), glm::vec3(1, 0, 0));
    mat = glm::scale(mat, s);
    mat = glm::translate(mat, -p);

    GeometryData outGeom = *inGeom;
    outGeom.Transform(mat);
    SetOutputGeometry(outGeom);
    return true;
}

} // namespace nf
