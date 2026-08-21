#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace nf {

struct ContactManifold {
    bool hasCollision = false;
    glm::vec3 contactPoint{0.0f};
    glm::vec3 normal{0.0f, 1.0f, 0.0f}; // Points from B to A
    float penetration = 0.0f;
};

class CollisionPrimitives {
public:
    static ContactManifold TestSphereSphere(
        const glm::vec3& posA, float radiusA,
        const glm::vec3& posB, float radiusB);

    static ContactManifold TestSpherePlane(
        const glm::vec3& spherePos, float radius,
        const glm::vec3& planePos, const glm::vec3& planeNormal);

    static ContactManifold TestBoxPlane(
        const glm::vec3& boxPos, const glm::vec3& boxExtents, const glm::quat& boxRot,
        const glm::vec3& planePos, const glm::vec3& planeNormal);

    static ContactManifold TestSphereBox(
        const glm::vec3& spherePos, float radius,
        const glm::vec3& boxPos, const glm::vec3& boxExtents, const glm::quat& boxRot);

    static ContactManifold TestBoxBox(
        const glm::vec3& posA, const glm::vec3& extentsA, const glm::quat& rotA,
        const glm::vec3& posB, const glm::vec3& extentsB, const glm::quat& rotB);
};

} // namespace nf
