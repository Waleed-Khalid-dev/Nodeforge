#pragma once

#include "../../graph/Node.h"
#include "../../gpu/Texture.h"

namespace nf {

class VolumetricCloudTexOp : public Node {
public:
    VolumetricCloudTexOp(NodeId id, const std::string& name);
    ~VolumetricCloudTexOp() override = default;

    bool Cook(const CookContext& context) override;

    float GetDensity() const;
    glm::vec3 GetSunDirection() const;
    glm::vec4 GetSunColor() const;
    float GetScatteringG() const;
    int32_t GetStepCount() const;
    float GetWindSpeed() const;

private:
    Pin* m_inEnvPin = nullptr;
    Pin* m_outTexPin = nullptr;
    std::shared_ptr<gpu::Texture2D> m_outTexture;
};

} // namespace nf
