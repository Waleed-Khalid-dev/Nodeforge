#pragma once

#include "../../graph/Node.h"
#include "../../core/GeometryData.h"
#include "../../physics/PhysicsWorld.h"

namespace nf {

class RigidBodyGeomOp : public Node {
public:
    RigidBodyGeomOp(NodeId id, const std::string& name);
    ~RigidBodyGeomOp() override = default;

    bool Cook(const CookContext& context) override;

    glm::vec3 GetPosition() const { return m_body.position; }
    glm::quat GetOrientation() const { return m_body.orientation; }
    glm::vec3 GetLinearVelocity() const { return m_body.linearVelocity; }

    void ApplyImpulse(const glm::vec3& impulse) { m_body.ApplyImpulse(impulse); }
    void ResetState();

private:
    Pin* m_inGeomPin = nullptr;
    Pin* m_inForcePin = nullptr;
    Pin* m_outGeomPin = nullptr;

    RigidBody m_body;
    GeometryData m_outGeometry;
    bool m_initialized = false;
};

} // namespace nf
