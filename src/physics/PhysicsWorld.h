#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "CollisionPrimitives.h"

namespace nf {

enum class BodyType {
    Dynamic = 0,
    Static = 1,
    Kinematic = 2
};

enum class ShapeType {
    Box = 0,
    Sphere = 1,
    Plane = 2,
    Convex = 3
};

struct RigidBody {
    uint32_t id = 0;
    BodyType type = BodyType::Dynamic;
    ShapeType shape = ShapeType::Box;
    glm::vec3 position{0.0f};
    glm::quat orientation{1.0f, 0.0f, 0.0f, 0.0f};
    glm::vec3 linearVelocity{0.0f};
    glm::vec3 angularVelocity{0.0f};
    glm::vec3 forceAccum{0.0f};
    glm::vec3 torqueAccum{0.0f};
    glm::vec3 size{1.0f}; // Dimensions or radius in x
    float mass = 1.0f;
    float invMass = 1.0f;
    float restitution = 0.5f;
    float friction = 0.3f;
    bool isSleeping = false;

    void ApplyForce(const glm::vec3& f) { forceAccum += f; }
    void ApplyImpulse(const glm::vec3& j) { linearVelocity += j * invMass; }
};

struct CollisionEvent {
    uint32_t bodyA = 0;
    uint32_t bodyB = 0;
    glm::vec3 contactPoint{0.0f};
    glm::vec3 normal{0.0f};
    float impactVelocity = 0.0f;
    float impactForce = 0.0f;
};

class PhysicsWorld {
public:
    PhysicsWorld();
    ~PhysicsWorld() = default;

    void Step(float dt, int subSteps = 4);
    void Reset();

    uint32_t AddBody(const RigidBody& body);
    RigidBody* GetBody(uint32_t id);
    const std::vector<RigidBody>& GetBodies() const { return m_bodies; }
    std::vector<RigidBody>& GetBodies() { return m_bodies; }

    void SetGravity(const glm::vec3& g) { m_gravity = g; }
    glm::vec3 GetGravity() const { return m_gravity; }

    void SetLinearDamping(float d) { m_linearDamping = d; }
    void SetAngularDamping(float d) { m_angularDamping = d; }

    const std::vector<CollisionEvent>& GetCollisionEvents() const { return m_recentEvents; }
    size_t GetBodyCount() const { return m_bodies.size(); }

private:
    void IntegrateForces(RigidBody& body, float dt);
    void IntegrateVelocities(RigidBody& body, float dt);
    void DetectAndResolveCollisions();

    glm::vec3 m_gravity{0.0f, -9.81f, 0.0f};
    float m_linearDamping = 0.02f;
    float m_angularDamping = 0.05f;
    std::vector<RigidBody> m_bodies;
    std::vector<CollisionEvent> m_recentEvents;
    uint32_t m_nextId = 1;
};

} // namespace nf
