#include "PhysicsSolverComp.h"
#include <algorithm>

namespace nf {

PhysicsSolverComp::PhysicsSolverComp(NodeId id, const std::string& name)
    : Node(id, name, "PhysicsSolverComp") {
    m_inBodiesPin = AddInputPin("in_bodies", PinType::Any);
    m_inForcesPin = AddInputPin("in_forces", PinType::Chan);
    m_outCompPin = AddOutputPin("output", PinType::Any);
    m_outEventsPin = AddOutputPin("out_events", PinType::Chan);

    SetParam("gravity", glm::vec3(0.0f, -9.81f, 0.0f));
    SetParam("sub_steps", static_cast<int32_t>(4));
    SetParam("linear_damping", 0.02f);
    SetParam("angular_damping", 0.05f);
    SetParam("time_scale", 1.0f);
    SetParam("reset", false);

    m_eventsBuffer.Resize(5, 1);
    m_eventsBuffer.SetChannelNames({"num_collisions", "impact_force", "contact_x", "contact_y", "contact_z"});
}

bool PhysicsSolverComp::Cook(const CookContext& context) {
    bool resetRequested = GetParam("reset").Is<bool>() && GetParam("reset").Get<bool>();
    if (resetRequested || !m_initialized) {
        m_world.Reset();
        m_initialized = true;
    }

    glm::vec3 gravity = GetParam("gravity").Is<glm::vec3>() ? GetParam("gravity").Get<glm::vec3>() : glm::vec3(0.0f, -9.81f, 0.0f);
    float linDamp = GetParam("linear_damping").Is<float>() ? GetParam("linear_damping").Get<float>() : 0.02f;
    float angDamp = GetParam("angular_damping").Is<float>() ? GetParam("angular_damping").Get<float>() : 0.05f;
    int32_t subSteps = GetParam("sub_steps").Is<int32_t>() ? GetParam("sub_steps").Get<int32_t>() : 4;
    float timeScale = GetParam("time_scale").Is<float>() ? GetParam("time_scale").Get<float>() : 1.0f;

    m_world.SetGravity(gravity);
    m_world.SetLinearDamping(linDamp);
    m_world.SetAngularDamping(angDamp);

    // Apply external force channel if connected
    if (m_inForcesPin && m_inForcesPin->GetValue().Is<ChannelBuffer>()) {
        const auto& fBuf = m_inForcesPin->GetValue().Get<ChannelBuffer>();
        if (fBuf.GetChannelCount() >= 3 && fBuf.GetSampleCount() > 0) {
            glm::vec3 extForce(fBuf.GetChannelData(0)[0], fBuf.GetChannelData(1)[0], fBuf.GetChannelData(2)[0]);
            for (auto& body : m_world.GetBodies()) {
                if (body.type == BodyType::Dynamic) {
                    body.ApplyForce(extForce);
                }
            }
        }
    }

    float dt = static_cast<float>(context.deltaTimeSeconds) * timeScale;
    if (dt <= 0.0f) dt = 1.0f / 60.0f;

    m_world.Step(dt, subSteps);

    // Aggregate collision events into output channel
    const auto& events = m_world.GetCollisionEvents();
    m_eventsBuffer.Resize(5, 1);
    m_eventsBuffer.SetChannelNames({"num_collisions", "impact_force", "contact_x", "contact_y", "contact_z"});

    m_eventsBuffer.GetChannelData(0)[0] = static_cast<float>(events.size());
    if (!events.empty()) {
        const auto& ev = events[0];
        m_eventsBuffer.GetChannelData(1)[0] = ev.impactForce;
        m_eventsBuffer.GetChannelData(2)[0] = ev.contactPoint.x;
        m_eventsBuffer.GetChannelData(3)[0] = ev.contactPoint.y;
        m_eventsBuffer.GetChannelData(4)[0] = ev.contactPoint.z;
    } else {
        m_eventsBuffer.GetChannelData(1)[0] = 0.0f;
        m_eventsBuffer.GetChannelData(2)[0] = 0.0f;
        m_eventsBuffer.GetChannelData(3)[0] = 0.0f;
        m_eventsBuffer.GetChannelData(4)[0] = 0.0f;
    }

    m_outEventsPin->SetValue(PinValue(m_eventsBuffer));
    return true;
}

} // namespace nf
