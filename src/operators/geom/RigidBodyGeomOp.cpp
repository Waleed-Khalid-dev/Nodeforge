#include "RigidBodyGeomOp.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

namespace nf {

RigidBodyGeomOp::RigidBodyGeomOp(NodeId id, const std::string& name)
    : Node(id, name, "RigidBodyGeomOp") {
    m_inGeomPin = AddInputPin("template_geom", PinType::Geom);
    m_inForcePin = AddInputPin("in_force", PinType::Chan);
    m_outGeomPin = AddOutputPin("output", PinType::Geom);

    SetParam("body_type", static_cast<int32_t>(0)); // 0: Dynamic, 1: Static, 2: Kinematic
    SetParam("mass", 1.0f);
    SetParam("restitution", 0.6f);
    SetParam("friction", 0.3f);
    SetParam("initial_pos", glm::vec3(0.0f, 5.0f, 0.0f));
    SetParam("initial_vel", glm::vec3(0.0f, 0.0f, 0.0f));
    SetParam("collision_shape", static_cast<int32_t>(0)); // 0: Box, 1: Sphere, 2: Plane, 3: Convex
}

void RigidBodyGeomOp::ResetState() {
    int32_t bodyType = GetParam("body_type").Is<int32_t>() ? GetParam("body_type").Get<int32_t>() : 0;
    float mass = GetParam("mass").Is<float>() ? GetParam("mass").Get<float>() : 1.0f;
    float restitution = GetParam("restitution").Is<float>() ? GetParam("restitution").Get<float>() : 0.6f;
    float friction = GetParam("friction").Is<float>() ? GetParam("friction").Get<float>() : 0.3f;
    glm::vec3 initPos = GetParam("initial_pos").Is<glm::vec3>() ? GetParam("initial_pos").Get<glm::vec3>() : glm::vec3(0.0f, 5.0f, 0.0f);
    glm::vec3 initVel = GetParam("initial_vel").Is<glm::vec3>() ? GetParam("initial_vel").Get<glm::vec3>() : glm::vec3(0.0f);

    m_body.type = static_cast<BodyType>(bodyType);
    m_body.mass = mass;
    m_body.invMass = (m_body.type == BodyType::Static || mass <= 0.0001f) ? 0.0f : (1.0f / mass);
    m_body.restitution = restitution;
    m_body.friction = friction;
    m_body.position = initPos;
    m_body.linearVelocity = initVel;
    m_body.orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    m_body.angularVelocity = glm::vec3(0.0f);
    m_body.forceAccum = glm::vec3(0.0f);
    m_body.torqueAccum = glm::vec3(0.0f);
    m_body.size = glm::vec3(1.0f);
    m_initialized = true;
}

bool RigidBodyGeomOp::Cook(const CookContext& context) {
    if (!m_initialized) {
        ResetState();
    }

    // Apply external force channel if connected
    if (m_inForcePin && m_inForcePin->GetValue().Is<ChannelBuffer>()) {
        const auto& fBuf = m_inForcePin->GetValue().Get<ChannelBuffer>();
        if (fBuf.GetChannelCount() >= 3 && fBuf.GetSampleCount() > 0) {
            glm::vec3 impulse(fBuf.GetChannelData(0)[0], fBuf.GetChannelData(1)[0], fBuf.GetChannelData(2)[0]);
            m_body.ApplyImpulse(impulse);
        }
    }

    float dt = static_cast<float>(context.deltaTimeSeconds);
    if (dt <= 0.0f) dt = 1.0f / 60.0f;

    // Simple integration step
    if (m_body.type == BodyType::Dynamic) {
        glm::vec3 gravity(0.0f, -9.81f, 0.0f);
        m_body.linearVelocity += gravity * dt;
        m_body.position += m_body.linearVelocity * dt;

        // Ground plane simple bounce if below y = 0
        if (m_body.position.y < 0.5f) {
            m_body.position.y = 0.5f;
            if (m_body.linearVelocity.y < 0.0f) {
                m_body.linearVelocity.y = -m_body.linearVelocity.y * m_body.restitution;
                m_body.linearVelocity.x *= (1.0f - m_body.friction);
                m_body.linearVelocity.z *= (1.0f - m_body.friction);
            }
        }
    }

    // Read input template geometry or generate unit cube
    GeometryData srcGeom;
    if (m_inGeomPin && m_inGeomPin->GetValue().Is<GeometryData>()) {
        srcGeom = m_inGeomPin->GetValue().Get<GeometryData>();
    } else {
        srcGeom = GeometryData::CreateBox(1.0f, 1.0f, 1.0f);
    }

    // Transform vertices by physical body position and rotation
    m_outGeometry = srcGeom;
    glm::mat4 modelMat = glm::translate(glm::mat4(1.0f), m_body.position) * glm::mat4_cast(m_body.orientation);
    m_outGeometry.Transform(modelMat);

    m_outGeomPin->SetValue(PinValue(m_outGeometry));
    return true;
}

} // namespace nf
