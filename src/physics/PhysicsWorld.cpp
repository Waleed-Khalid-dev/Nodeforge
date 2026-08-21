#include "PhysicsWorld.h"
#include <algorithm>
#include <cmath>

namespace nf {

PhysicsWorld::PhysicsWorld() {
    m_bodies.reserve(1024);
}

void PhysicsWorld::Reset() {
    m_bodies.clear();
    m_recentEvents.clear();
    m_nextId = 1;
}

uint32_t PhysicsWorld::AddBody(const RigidBody& body) {
    RigidBody b = body;
    b.id = m_nextId++;
    if (b.type == BodyType::Static) {
        b.invMass = 0.0f;
    } else {
        b.invMass = (b.mass > 0.0001f) ? (1.0f / b.mass) : 1.0f;
    }
    m_bodies.push_back(b);
    return b.id;
}

RigidBody* PhysicsWorld::GetBody(uint32_t id) {
    for (auto& b : m_bodies) {
        if (b.id == id) return &b;
    }
    return nullptr;
}

void PhysicsWorld::IntegrateForces(RigidBody& body, float dt) {
    if (body.type != BodyType::Dynamic || body.isSleeping) return;

    // Gravity & external force accumulation
    glm::vec3 totalAccel = m_gravity + body.forceAccum * body.invMass;
    body.linearVelocity += totalAccel * dt;

    // Damping
    body.linearVelocity *= std::max(0.0f, 1.0f - m_linearDamping * dt);
    body.angularVelocity *= std::max(0.0f, 1.0f - m_angularDamping * dt);

    body.forceAccum = glm::vec3(0.0f);
    body.torqueAccum = glm::vec3(0.0f);
}

void PhysicsWorld::IntegrateVelocities(RigidBody& body, float dt) {
    if (body.type != BodyType::Dynamic || body.isSleeping) return;

    body.position += body.linearVelocity * dt;

    // Angular orientation update
    float angSpeed = glm::length(body.angularVelocity);
    if (angSpeed > 0.0001f) {
        glm::vec3 axis = body.angularVelocity / angSpeed;
        glm::quat deltaRot = glm::angleAxis(angSpeed * dt, axis);
        body.orientation = glm::normalize(deltaRot * body.orientation);
    }
}

void PhysicsWorld::DetectAndResolveCollisions() {
    size_t count = m_bodies.size();
    for (size_t i = 0; i < count; ++i) {
        for (size_t j = i + 1; j < count; ++j) {
            RigidBody& a = m_bodies[i];
            RigidBody& b = m_bodies[j];

            if (a.type == BodyType::Static && b.type == BodyType::Static) continue;

            ContactManifold manifold;

            if (a.shape == ShapeType::Sphere && b.shape == ShapeType::Sphere) {
                manifold = CollisionPrimitives::TestSphereSphere(a.position, a.size.x, b.position, b.size.x);
            } else if (a.shape == ShapeType::Sphere && b.shape == ShapeType::Plane) {
                manifold = CollisionPrimitives::TestSpherePlane(a.position, a.size.x, b.position, b.size);
            } else if (a.shape == ShapeType::Box && b.shape == ShapeType::Plane) {
                manifold = CollisionPrimitives::TestBoxPlane(a.position, a.size, a.orientation, b.position, b.size);
            } else if (a.shape == ShapeType::Sphere && b.shape == ShapeType::Box) {
                manifold = CollisionPrimitives::TestSphereBox(a.position, a.size.x, b.position, b.size, b.orientation);
            } else if (a.shape == ShapeType::Box && b.shape == ShapeType::Box) {
                manifold = CollisionPrimitives::TestBoxBox(a.position, a.size, a.orientation, b.position, b.size, b.orientation);
            }

            if (manifold.hasCollision) {
                // Positional correction (resolve penetration)
                float totalInvMass = a.invMass + b.invMass;
                if (totalInvMass > 0.0001f) {
                    glm::vec3 correction = manifold.normal * (manifold.penetration / totalInvMass) * 0.8f;
                    if (a.type == BodyType::Dynamic) a.position += correction * a.invMass;
                    if (b.type == BodyType::Dynamic) b.position -= correction * b.invMass;
                }

                // Relative velocity along normal
                glm::vec3 relVel = a.linearVelocity - b.linearVelocity;
                float velAlongNormal = glm::dot(relVel, manifold.normal);

                if (velAlongNormal < 0.0f) {
                    float restitution = std::min(a.restitution, b.restitution);
                    float jImpulse = -(1.0f + restitution) * velAlongNormal / totalInvMass;

                    glm::vec3 impulse = manifold.normal * jImpulse;
                    if (a.type == BodyType::Dynamic) a.linearVelocity += impulse * a.invMass;
                    if (b.type == BodyType::Dynamic) b.linearVelocity -= impulse * b.invMass;

                    // Friction tangential impulse
                    glm::vec3 tangent = relVel - manifold.normal * velAlongNormal;
                    float tangentLen = glm::length(tangent);
                    if (tangentLen > 0.0001f) {
                        tangent /= tangentLen;
                        float friction = std::sqrt(a.friction * b.friction);
                        float jTangent = -glm::dot(relVel, tangent) / totalInvMass;
                        jTangent = std::clamp(jTangent, -jImpulse * friction, jImpulse * friction);
                        glm::vec3 frictionImpulse = tangent * jTangent;
                        if (a.type == BodyType::Dynamic) a.linearVelocity += frictionImpulse * a.invMass;
                        if (b.type == BodyType::Dynamic) b.linearVelocity -= frictionImpulse * b.invMass;
                    }

                    // Record collision event
                    CollisionEvent ev;
                    ev.bodyA = a.id;
                    ev.bodyB = b.id;
                    ev.contactPoint = manifold.contactPoint;
                    ev.normal = manifold.normal;
                    ev.impactVelocity = std::abs(velAlongNormal);
                    ev.impactForce = jImpulse;
                    m_recentEvents.push_back(ev);
                }
            }
        }
    }
}

void PhysicsWorld::Step(float dt, int subSteps) {
    m_recentEvents.clear();
    int steps = std::clamp(subSteps, 1, 16);
    float subDt = dt / static_cast<float>(steps);

    for (int s = 0; s < steps; ++s) {
        for (auto& body : m_bodies) {
            IntegrateForces(body, subDt);
        }

        DetectAndResolveCollisions();

        for (auto& body : m_bodies) {
            IntegrateVelocities(body, subDt);
        }
    }
}

} // namespace nf
