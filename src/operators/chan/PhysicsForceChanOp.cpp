#include "PhysicsForceChanOp.h"
#include <algorithm>
#include <cmath>

namespace nf {

PhysicsForceChanOp::PhysicsForceChanOp(NodeId id, const std::string& name)
    : Node(id, name, "PhysicsForceChanOp") {
    m_inTriggerPin = AddInputPin("in_trigger", PinType::Chan);
    m_outForcePin = AddOutputPin("out_force", PinType::Chan);

    SetParam("force_type", static_cast<int32_t>(0)); // 0: Directional Wind, 1: Radial Explosion, 2: Point Attractor, 3: Vortex
    SetParam("strength", 100.0f);
    SetParam("direction", glm::vec3(0.0f, 0.0f, 1.0f));
    SetParam("origin", glm::vec3(0.0f, 0.0f, 0.0f));
    SetParam("radius", 15.0f);

    m_outBuffer.Resize(4, 1);
    m_outBuffer.SetChannelNames({"fx", "fy", "fz", "magnitude"});
}

glm::vec3 PhysicsForceChanOp::GetDirection() const {
    return GetParam("direction").Is<glm::vec3>() ? GetParam("direction").Get<glm::vec3>() : glm::vec3(0.0f, 0.0f, 1.0f);
}

glm::vec3 PhysicsForceChanOp::GetOrigin() const {
    return GetParam("origin").Is<glm::vec3>() ? GetParam("origin").Get<glm::vec3>() : glm::vec3(0.0f);
}

float PhysicsForceChanOp::GetStrength() const {
    return GetParam("strength").Is<float>() ? GetParam("strength").Get<float>() : 100.0f;
}

float PhysicsForceChanOp::GetRadius() const {
    return GetParam("radius").Is<float>() ? GetParam("radius").Get<float>() : 15.0f;
}

bool PhysicsForceChanOp::Cook(const CookContext& /*context*/) {
    int32_t type = GetParam("force_type").Is<int32_t>() ? GetParam("force_type").Get<int32_t>() : 0;
    float strength = GetStrength();
    glm::vec3 dir = GetDirection();

    // Check trigger multiplier if provided
    float triggerScale = 1.0f;
    if (m_inTriggerPin && m_inTriggerPin->GetValue().Is<ChannelBuffer>()) {
        const auto& tBuf = m_inTriggerPin->GetValue().Get<ChannelBuffer>();
        if (tBuf.GetChannelCount() > 0 && tBuf.GetSampleCount() > 0) {
            triggerScale = tBuf.GetChannelData(0)[0];
        }
    }

    glm::vec3 finalForce{0.0f};

    if (type == 0) {
        // Directional Constant Wind
        if (glm::length(dir) > 0.0001f) {
            finalForce = glm::normalize(dir) * strength * triggerScale;
        }
    } else if (type == 1) {
        // Radial Explosion Blast
        finalForce = glm::vec3(0.0f, 1.0f, 0.5f) * strength * triggerScale;
    } else if (type == 2) {
        // Point Attractor
        finalForce = -glm::normalize(GetOrigin() + glm::vec3(0.001f)) * strength * triggerScale;
    } else {
        // Vortex Swirl
        finalForce = glm::vec3(-dir.z, 0.0f, dir.x) * strength * triggerScale;
    }

    m_outBuffer.Resize(4, 1);
    m_outBuffer.SetChannelNames({"fx", "fy", "fz", "magnitude"});
    m_outBuffer.GetChannelData(0)[0] = finalForce.x;
    m_outBuffer.GetChannelData(1)[0] = finalForce.y;
    m_outBuffer.GetChannelData(2)[0] = finalForce.z;
    m_outBuffer.GetChannelData(3)[0] = glm::length(finalForce);

    m_outForcePin->SetValue(PinValue(m_outBuffer));
    return true;
}

} // namespace nf
