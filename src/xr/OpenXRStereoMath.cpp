#include "OpenXRStereoMath.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

namespace nf {

glm::mat4 OpenXRStereoMath::ComputeProjectionMatrix(const OpenXRFov& fov, float nearZ, float farZ) {
    float tanL = std::tan(fov.angleLeft);
    float tanR = std::tan(fov.angleRight);
    float tanU = std::tan(fov.angleUp);
    float tanD = std::tan(fov.angleDown);

    glm::mat4 proj(0.0f);
    proj[0][0] = 2.0f / (tanR - tanL);
    proj[2][0] = (tanR + tanL) / (tanR - tanL);

    proj[1][1] = 2.0f / (tanU - tanD);
    proj[2][1] = (tanU + tanD) / (tanU - tanD);

    proj[2][2] = -(farZ + nearZ) / (farZ - nearZ);
    proj[3][2] = -(2.0f * farZ * nearZ) / (farZ - nearZ);

    proj[2][3] = -1.0f;
    return proj;
}

glm::mat4 OpenXRStereoMath::ComputeViewMatrix(const glm::vec3& headPos, const glm::quat& headRot, bool isLeftEye, float ipdMeters) {
    float eyeOffset = isLeftEye ? (-ipdMeters * 0.5f) : (ipdMeters * 0.5f);
    glm::vec3 localOffset(eyeOffset, 0.0f, 0.0f);
    glm::vec3 eyePos = headPos + headRot * localOffset;

    glm::mat4 rotMat = glm::mat4_cast(headRot);
    glm::mat4 viewMat = glm::transpose(rotMat) * glm::translate(glm::mat4(1.0f), -eyePos);
    return viewMat;
}

} // namespace nf
