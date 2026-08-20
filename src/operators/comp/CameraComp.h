#pragma once

#include "../../graph/Node.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace nf {

class CameraComp : public Node {
public:
    CameraComp(NodeId id, const std::string& name);

    bool Cook(const CookContext& context) override;

    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix(float aspect) const;

    glm::vec3 GetPosition() const;
    glm::vec3 GetLookAt() const;
    glm::vec3 GetUpVector() const;
    float GetFOV() const;
    float GetNearPlane() const;
    float GetFarPlane() const;
    int GetProjectionType() const; // 0: Perspective, 1: Orthographic

private:
    Pin* m_outPin = nullptr;
};

} // namespace nf
