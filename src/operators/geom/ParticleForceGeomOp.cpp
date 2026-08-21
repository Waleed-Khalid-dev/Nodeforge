#include "ParticleForceGeomOp.h"
#include <cmath>

namespace nf {

ParticleForceGeomOp::ParticleForceGeomOp(NodeId id, const std::string& name)
    : GeomOp(id, name, "ParticleForceGeomOp") {
    m_inGeomPin = AddInputPin("input", PinType::Geom);
    m_inForceModPin = AddInputPin("force_mod", PinType::Chan);
    m_outPin = AddOutputPin("output", PinType::Geom);

    SetParam("gravity", glm::vec3(0.0f, -1.0f, 0.0f));
    SetParam("drag", 0.15f);
    SetParam("turbulence_strength", 1.5f);
    SetParam("turbulence_frequency", 0.5f);
    SetParam("turbulence_speed", 0.3f);
    SetParam("vortex_axis", glm::vec3(0.0f, 1.0f, 0.0f));
    SetParam("vortex_strength", 0.0f);
    SetParam("collision_floor", true);
    SetParam("floor_height", 0.0f);
    SetParam("restitution", 0.6f);
}

bool ParticleForceGeomOp::Cook(const CookContext& context) {
    const GeometryData* inGeom = GetInputGeometry(0);
    if (!inGeom) {
        SetOutputGeometry(GeometryData{});
        return true;
    }

    GeometryData outGeom = *inGeom;
    float dt = static_cast<float>(std::max(0.0001, context.deltaTimeSeconds));
    float time = static_cast<float>(context.timeSeconds);

    glm::vec3 gravity = GetParam("gravity").Is<glm::vec3>() ? GetParam("gravity").Get<glm::vec3>() : glm::vec3(0.0f, -1.0f, 0.0f);
    float drag = GetParam("drag").Is<float>() ? GetParam("drag").Get<float>() : 0.15f;
    float turbStr = GetParam("turbulence_strength").Is<float>() ? GetParam("turbulence_strength").Get<float>() : 1.5f;
    float turbFreq = GetParam("turbulence_frequency").Is<float>() ? GetParam("turbulence_frequency").Get<float>() : 0.5f;
    float turbSpd = GetParam("turbulence_speed").Is<float>() ? GetParam("turbulence_speed").Get<float>() : 0.3f;
    glm::vec3 vortexAxis = GetParam("vortex_axis").Is<glm::vec3>() ? GetParam("vortex_axis").Get<glm::vec3>() : glm::vec3(0.0f, 1.0f, 0.0f);
    float vortexStr = GetParam("vortex_strength").Is<float>() ? GetParam("vortex_strength").Get<float>() : 0.0f;
    bool colFloor = GetParam("collision_floor").Is<bool>() ? GetParam("collision_floor").Get<bool>() : true;
    float floorH = GetParam("floor_height").Is<float>() ? GetParam("floor_height").Get<float>() : 0.0f;

    auto& vertices = outGeom.GetVertices();
    for (auto& v : vertices) {
        glm::vec3 accel = gravity;

        if (turbStr > 0.0f) {
            glm::vec3 coord = v.pos * turbFreq + glm::vec3(0.0f, 0.0f, time * turbSpd);
            float curlX = std::sin(coord.y * 3.14f) * std::cos(coord.z * 3.14f);
            float curlY = std::sin(coord.z * 3.14f) * std::cos(coord.x * 3.14f);
            float curlZ = std::sin(coord.x * 3.14f) * std::cos(coord.y * 3.14f);
            accel += glm::vec3(curlX, curlY, curlZ) * turbStr * 5.0f;
        }

        if (std::abs(vortexStr) > 1e-4f && glm::length(vortexAxis) > 1e-4f) {
            glm::vec3 ax = glm::normalize(vortexAxis);
            glm::vec3 rad = v.pos - ax * glm::dot(v.pos, ax);
            glm::vec3 tanDir = glm::cross(ax, rad);
            if (glm::length(tanDir) > 1e-4f) {
                accel += glm::normalize(tanDir) * vortexStr;
            }
        }

        v.pos += accel * dt * dt * 0.5f;
        v.pos *= std::max(0.0f, 1.0f - drag * dt * 0.1f);

        if (colFloor && v.pos.y < floorH) {
            v.pos.y = floorH;
        }
    }

    SetOutputGeometry(std::move(outGeom));
    return true;
}

} // namespace nf
