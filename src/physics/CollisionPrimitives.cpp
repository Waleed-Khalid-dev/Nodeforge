#include "CollisionPrimitives.h"
#include <algorithm>
#include <cmath>

namespace nf {

ContactManifold CollisionPrimitives::TestSphereSphere(
    const glm::vec3& posA, float radiusA,
    const glm::vec3& posB, float radiusB) {
    ContactManifold manifold;
    glm::vec3 delta = posA - posB;
    float dist = glm::length(delta);
    float minDist = radiusA + radiusB;

    if (dist < minDist) {
        manifold.hasCollision = true;
        manifold.penetration = minDist - dist;
        manifold.normal = (dist > 0.0001f) ? glm::normalize(delta) : glm::vec3(0.0f, 1.0f, 0.0f);
        manifold.contactPoint = posB + manifold.normal * radiusB;
    }
    return manifold;
}

ContactManifold CollisionPrimitives::TestSpherePlane(
    const glm::vec3& spherePos, float radius,
    const glm::vec3& planePos, const glm::vec3& planeNormal) {
    ContactManifold manifold;
    glm::vec3 n = glm::normalize(planeNormal);
    float dist = glm::dot(spherePos - planePos, n);

    if (dist < radius) {
        manifold.hasCollision = true;
        manifold.penetration = radius - dist;
        manifold.normal = n;
        manifold.contactPoint = spherePos - n * dist;
    }
    return manifold;
}

ContactManifold CollisionPrimitives::TestBoxPlane(
    const glm::vec3& boxPos, const glm::vec3& boxExtents, const glm::quat& boxRot,
    const glm::vec3& planePos, const glm::vec3& planeNormal) {
    ContactManifold manifold;
    glm::vec3 n = glm::normalize(planeNormal);

    // Compute 8 oriented box corners
    glm::mat3 rotMat = glm::mat3_cast(boxRot);
    glm::vec3 half = boxExtents * 0.5f;

    float minDistance = 1e9f;
    glm::vec3 deepestCorner{0.0f};

    for (int x = -1; x <= 1; x += 2) {
        for (int y = -1; y <= 1; y += 2) {
            for (int z = -1; z <= 1; z += 2) {
                glm::vec3 localCorner = glm::vec3(x, y, z) * half;
                glm::vec3 worldCorner = boxPos + rotMat * localCorner;
                float d = glm::dot(worldCorner - planePos, n);
                if (d < minDistance) {
                    minDistance = d;
                    deepestCorner = worldCorner;
                }
            }
        }
    }

    if (minDistance < 0.0f) {
        manifold.hasCollision = true;
        manifold.penetration = -minDistance;
        manifold.normal = n;
        manifold.contactPoint = deepestCorner;
    }
    return manifold;
}

ContactManifold CollisionPrimitives::TestSphereBox(
    const glm::vec3& spherePos, float radius,
    const glm::vec3& boxPos, const glm::vec3& boxExtents, const glm::quat& boxRot) {
    ContactManifold manifold;
    glm::mat3 invRot = glm::transpose(glm::mat3_cast(boxRot));
    glm::vec3 localSpherePos = invRot * (spherePos - boxPos);
    glm::vec3 half = boxExtents * 0.5f;

    // Clamped closest point in box local space
    glm::vec3 closestPointLocal = glm::clamp(localSpherePos, -half, half);
    glm::vec3 deltaLocal = localSpherePos - closestPointLocal;
    float dist = glm::length(deltaLocal);

    if (dist < radius) {
        manifold.hasCollision = true;
        manifold.penetration = radius - dist;
        glm::mat3 rotMat = glm::mat3_cast(boxRot);
        glm::vec3 normalLocal = (dist > 0.0001f) ? glm::normalize(deltaLocal) : glm::vec3(0.0f, 1.0f, 0.0f);
        manifold.normal = rotMat * normalLocal;
        manifold.contactPoint = boxPos + rotMat * closestPointLocal;
    }
    return manifold;
}

ContactManifold CollisionPrimitives::TestBoxBox(
    const glm::vec3& posA, const glm::vec3& extentsA, const glm::quat& rotA,
    const glm::vec3& posB, const glm::vec3& extentsB, const glm::quat& rotB) {
    ContactManifold manifold;
    glm::vec3 halfA = extentsA * 0.5f;
    glm::vec3 halfB = extentsB * 0.5f;

    glm::mat3 rA = glm::mat3_cast(rotA);
    glm::mat3 rB = glm::mat3_cast(rotB);

    glm::vec3 delta = posA - posB;

    // Fast SAT axis check along standard coordinate axes
    glm::vec3 absDelta = glm::abs(delta);
    glm::vec3 totalHalf = halfA + halfB;

    if (absDelta.x < totalHalf.x && absDelta.y < totalHalf.y && absDelta.z < totalHalf.z) {
        manifold.hasCollision = true;
        glm::vec3 overlap = totalHalf - absDelta;

        if (overlap.x <= overlap.y && overlap.x <= overlap.z) {
            manifold.penetration = overlap.x;
            manifold.normal = glm::vec3((delta.x > 0.0f) ? 1.0f : -1.0f, 0.0f, 0.0f);
        } else if (overlap.y <= overlap.x && overlap.y <= overlap.z) {
            manifold.penetration = overlap.y;
            manifold.normal = glm::vec3(0.0f, (delta.y > 0.0f) ? 1.0f : -1.0f, 0.0f);
        } else {
            manifold.penetration = overlap.z;
            manifold.normal = glm::vec3(0.0f, 0.0f, (delta.z > 0.0f) ? 1.0f : -1.0f);
        }
        manifold.contactPoint = posB + manifold.normal * (glm::dot(halfB, glm::abs(manifold.normal)));
    }
    return manifold;
}

} // namespace nf
