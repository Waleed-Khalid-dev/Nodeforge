#include "OpenXRRuntime.h"

namespace nf {

OpenXRRuntime& OpenXRRuntime::Instance() {
    static OpenXRRuntime instance;
    return instance;
}

OpenXRRuntime::OpenXRRuntime() {
    m_leftController.position = glm::vec3(-0.25f, 1.2f, -0.4f);
    m_rightController.position = glm::vec3(0.25f, 1.2f, -0.4f);

    m_leftHand.joints.resize(26);
    m_rightHand.joints.resize(26);
    for (size_t i = 0; i < 26; ++i) {
        m_leftHand.joints[i].position = m_leftController.position + glm::vec3(0.01f * static_cast<float>(i), 0.0f, 0.0f);
        m_rightHand.joints[i].position = m_rightController.position + glm::vec3(0.01f * static_cast<float>(i), 0.0f, 0.0f);
    }
}

OpenXRRuntime::~OpenXRRuntime() {
    Shutdown();
}

bool OpenXRRuntime::Initialize(bool emulateHeadless) {
    m_isEmulated = emulateHeadless;
    m_initialized = true;
    return true;
}

void OpenXRRuntime::Shutdown() {
    m_initialized = false;
}

void OpenXRRuntime::PollEvents() {
    // In emulated/active mode, update internal tick timestamps
}

void OpenXRRuntime::SetEmulatedHeadPose(const glm::vec3& pos, const glm::quat& rot) {
    m_headPosition = pos;
    m_headOrientation = rot;
}

const XRControllerState& OpenXRRuntime::GetController(XRHand hand) const {
    return (hand == XRHand::Left) ? m_leftController : m_rightController;
}

void OpenXRRuntime::SetEmulatedController(XRHand hand, const XRControllerState& state) {
    if (hand == XRHand::Left) {
        m_leftController = state;
    } else {
        m_rightController = state;
    }
}

const XRHandTrackingState& OpenXRRuntime::GetHandTracking(XRHand hand) const {
    return (hand == XRHand::Left) ? m_leftHand : m_rightHand;
}

void OpenXRRuntime::SetEmulatedHandTracking(XRHand hand, const XRHandTrackingState& state) {
    if (hand == XRHand::Left) {
        m_leftHand = state;
    } else {
        m_rightHand = state;
    }
}

glm::mat4 OpenXRRuntime::GetProjectionMatrix(bool isLeftEye) const {
    const auto& fov = isLeftEye ? m_leftFov : m_rightFov;
    return OpenXRStereoMath::ComputeProjectionMatrix(fov);
}

glm::mat4 OpenXRRuntime::GetViewMatrix(bool isLeftEye) const {
    return OpenXRStereoMath::ComputeViewMatrix(m_headPosition, m_headOrientation, isLeftEye, m_ipdMeters);
}

} // namespace nf
