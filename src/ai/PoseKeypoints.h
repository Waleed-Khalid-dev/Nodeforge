#pragma once

#include <glm/glm.hpp>
#include <array>
#include <string>

namespace nf {

enum class BodyJoint : uint32_t {
    Nose = 0,
    LeftEye = 1,
    RightEye = 2,
    LeftEar = 3,
    RightEar = 4,
    LeftShoulder = 5,
    RightShoulder = 6,
    LeftElbow = 7,
    RightElbow = 8,
    LeftWrist = 9,
    RightWrist = 10,
    LeftHip = 11,
    RightHip = 12,
    LeftKnee = 13,
    RightKnee = 14,
    LeftAnkle = 15,
    RightAnkle = 16,
    Count = 17
};

struct Keypoint2D {
    glm::vec2 position{0.0f};
    float confidence = 1.0f;
};

struct Pose17 {
    std::array<Keypoint2D, 17> keypoints{};
};

} // namespace nf
