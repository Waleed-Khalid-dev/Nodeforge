#include "CameraComp.h"

namespace nf {

CameraComp::CameraComp(NodeId id, const std::string& name)
    : Node(id, name, "CameraComp") {
    m_outPin = AddOutputPin("output", PinType::Any);

    SetParam("projection_type", static_cast<int32_t>(0)); // 0: Perspective, 1: Ortho
    SetParam("fov_y", 45.0f);
    SetParam("near_plane", 0.1f);
    SetParam("far_plane", 1000.0f);
    SetParam("position", glm::vec3(0.0f, 0.0f, 5.0f));
    SetParam("look_at", glm::vec3(0.0f, 0.0f, 0.0f));
    SetParam("up_vector", glm::vec3(0.0f, 1.0f, 0.0f));
}

bool CameraComp::Cook(const CookContext& /*context*/) {
    return true;
}

glm::vec3 CameraComp::GetPosition() const {
    return GetParam("position").Is<glm::vec3>() ? GetParam("position").Get<glm::vec3>() : glm::vec3(0.0f, 0.0f, 5.0f);
}

glm::vec3 CameraComp::GetLookAt() const {
    return GetParam("look_at").Is<glm::vec3>() ? GetParam("look_at").Get<glm::vec3>() : glm::vec3(0.0f, 0.0f, 0.0f);
}

glm::vec3 CameraComp::GetUpVector() const {
    return GetParam("up_vector").Is<glm::vec3>() ? GetParam("up_vector").Get<glm::vec3>() : glm::vec3(0.0f, 1.0f, 0.0f);
}

float CameraComp::GetFOV() const {
    return GetParam("fov_y").Is<float>() ? GetParam("fov_y").Get<float>() : 45.0f;
}

float CameraComp::GetNearPlane() const {
    return GetParam("near_plane").Is<float>() ? GetParam("near_plane").Get<float>() : 0.1f;
}

float CameraComp::GetFarPlane() const {
    return GetParam("far_plane").Is<float>() ? GetParam("far_plane").Get<float>() : 1000.0f;
}

int CameraComp::GetProjectionType() const {
    return GetParam("projection_type").Is<int32_t>() ? GetParam("projection_type").Get<int32_t>() : 0;
}

glm::mat4 CameraComp::GetViewMatrix() const {
    return glm::lookAt(GetPosition(), GetLookAt(), GetUpVector());
}

glm::mat4 CameraComp::GetProjectionMatrix(float aspect) const {
    aspect = std::max(0.001f, aspect);
    if (GetProjectionType() == 1) { // Ortho
        float orthoSize = 5.0f;
        float h = orthoSize * 0.5f;
        float w = h * aspect;
        return glm::ortho(-w, w, -h, h, GetNearPlane(), GetFarPlane());
    } else { // Perspective
        glm::mat4 proj = glm::perspective(glm::radians(GetFOV()), aspect, GetNearPlane(), GetFarPlane());
        // Vulkan clip space has inverted Y compared to OpenGL
        proj[1][1] *= -1.0f;
        return proj;
    }
}

} // namespace nf
