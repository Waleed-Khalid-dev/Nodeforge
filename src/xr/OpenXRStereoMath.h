#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace nf {

struct OpenXRFov {
    float angleLeft = -0.872665f;  // -50 deg in radians
    float angleRight = 0.872665f;  // +50 deg in radians
    float angleUp = 0.872665f;     // +50 deg in radians
    float angleDown = -0.872665f;  // -50 deg in radians
};

class OpenXRStereoMath {
public:
    static glm::mat4 ComputeProjectionMatrix(const OpenXRFov& fov, float nearZ = 0.05f, float farZ = 1000.0f);
    static glm::mat4 ComputeViewMatrix(const glm::vec3& headPos, const glm::quat& headRot, bool isLeftEye, float ipdMeters = 0.064f);
};

} // namespace nf
