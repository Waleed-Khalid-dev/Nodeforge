#include "ParticleAttractorGeomOp.h"
#include "../../core/ChannelBuffer.h"
#include <cmath>

namespace nf {

ParticleAttractorGeomOp::ParticleAttractorGeomOp(NodeId id, const std::string& name)
    : GeomOp(id, name, "ParticleAttractorGeomOp") {
    m_inGeomPin = AddInputPin("input", PinType::Geom);
    m_inTargetPin = AddInputPin("target_pos", PinType::Chan);
    m_outPin = AddOutputPin("output", PinType::Geom);

    SetParam("attractor_type", static_cast<int32_t>(0)); // 0: Attract, 1: Repel
    SetParam("position", glm::vec3(0.0f, 0.0f, 0.0f));
    SetParam("strength", 5.0f);
    SetParam("radius", 4.0f);
    SetParam("falloff", static_cast<int32_t>(1)); // 0: Linear, 1: Inverse Square, 2: Smoothstep
    SetParam("max_velocity", 20.0f);
}

bool ParticleAttractorGeomOp::Cook(const CookContext& context) {
    const GeometryData* inGeom = GetInputGeometry(0);
    if (!inGeom) {
        SetOutputGeometry(GeometryData{});
        return true;
    }

    GeometryData outGeom = *inGeom;
    float dt = static_cast<float>(std::max(0.0001, context.deltaTimeSeconds));

    glm::vec3 targetPos = GetParam("position").Is<glm::vec3>() ? GetParam("position").Get<glm::vec3>() : glm::vec3(0.0f);

    // If channel input connected, read target position from channels
    if (m_inTargetPin && m_inTargetPin->IsConnected()) {
        const auto& buf = m_inTargetPin->GetValue().Get<ChannelBuffer>();
        if (buf.GetChannelCount() >= 3 && buf.GetSampleCount() > 0) {
            targetPos.x = buf.GetChannelData(0)[0];
            targetPos.y = buf.GetChannelData(1)[0];
            targetPos.z = buf.GetChannelData(2)[0];
        } else if (buf.GetChannelCount() >= 2 && buf.GetSampleCount() > 0) {
            targetPos.x = buf.GetChannelData(0)[0];
            targetPos.y = buf.GetChannelData(1)[0];
        }
    }

    int32_t attrType = GetParam("attractor_type").Is<int32_t>() ? GetParam("attractor_type").Get<int32_t>() : 0;
    float strength = GetParam("strength").Is<float>() ? GetParam("strength").Get<float>() : 5.0f;
    float radius = GetParam("radius").Is<float>() ? GetParam("radius").Get<float>() : 4.0f;
    int32_t falloff = GetParam("falloff").Is<int32_t>() ? GetParam("falloff").Get<int32_t>() : 1;

    float sign = (attrType == 0) ? 1.0f : -1.0f;

    auto& vertices = outGeom.GetVertices();
    for (auto& v : vertices) {
        glm::vec3 toTarget = targetPos - v.pos;
        float dist = glm::length(toTarget);

        if (dist < radius && dist > 0.01f) {
            glm::vec3 dir = toTarget / dist;
            float factor = 1.0f - (dist / radius);
            if (falloff == 1) {
                factor = 1.0f / (dist * dist + 0.1f);
            } else if (falloff == 2) {
                factor = factor * factor * (3.0f - 2.0f * factor); // Smoothstep
            }

            glm::vec3 force = dir * (strength * factor * sign * 10.0f);
            v.pos += force * dt * dt * 0.5f;
        }
    }

    SetOutputGeometry(std::move(outGeom));
    return true;
}

} // namespace nf
