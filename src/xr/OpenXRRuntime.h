#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include <string>
#include "OpenXRStereoMath.h"

namespace nf {

enum class XRHand {
    Left = 0,
    Right = 1
};

struct XRControllerState {
    bool isTracked = true;
    glm::vec3 position{0.0f};
    glm::quat orientation{1.0f, 0.0f, 0.0f, 0.0f};
    float trigger = 0.0f;
    float grip = 0.0f;
    glm::vec2 thumbstick{0.0f};
    bool primaryButton = false;
    bool secondaryButton = false;
    bool thumbstickClick = false;
    float batteryLevel = 1.0f;
};

struct XRHandJoint {
    glm::vec3 position{0.0f};
    glm::quat orientation{1.0f, 0.0f, 0.0f, 0.0f};
    float radius = 0.01f;
};

struct XRHandTrackingState {
    bool isTracked = true;
    float pinchStrength = 0.0f;
    float grabStrength = 0.0f;
    glm::vec3 palmNormal{0.0f, -1.0f, 0.0f};
    std::vector<XRHandJoint> joints; // 26 joints
};

class OpenXRRuntime {
public:
    static OpenXRRuntime& Instance();

    bool Initialize(bool emulateHeadless = true);
    void Shutdown();
    bool IsActive() const { return m_initialized; }

    void PollEvents();

    // Headset Tracking
    glm::vec3 GetHeadPosition() const { return m_headPosition; }
    glm::quat GetHeadOrientation() const { return m_headOrientation; }
    void SetEmulatedHeadPose(const glm::vec3& pos, const glm::quat& rot);

    // Controller & Hand Tracking
    const XRControllerState& GetController(XRHand hand) const;
    void SetEmulatedController(XRHand hand, const XRControllerState& state);

    const XRHandTrackingState& GetHandTracking(XRHand hand) const;
    void SetEmulatedHandTracking(XRHand hand, const XRHandTrackingState& state);

    // Optics & Matrix Math
    float GetIPD() const { return m_ipdMeters; }
    void SetIPD(float ipdMeters) { m_ipdMeters = ipdMeters; }
    glm::mat4 GetProjectionMatrix(bool isLeftEye) const;
    glm::mat4 GetViewMatrix(bool isLeftEye) const;

private:
    OpenXRRuntime();
    ~OpenXRRuntime();

    bool m_initialized = false;
    bool m_isEmulated = true;
    float m_ipdMeters = 0.064f;

    glm::vec3 m_headPosition{0.0f, 1.7f, 0.0f}; // Default standing eye height
    glm::quat m_headOrientation{1.0f, 0.0f, 0.0f, 0.0f};

    XRControllerState m_leftController;
    XRControllerState m_rightController;
    XRHandTrackingState m_leftHand;
    XRHandTrackingState m_rightHand;
    OpenXRFov m_leftFov;
    OpenXRFov m_rightFov;
};

} // namespace nf
