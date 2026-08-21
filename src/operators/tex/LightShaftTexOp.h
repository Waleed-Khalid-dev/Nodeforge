#pragma once

#include "../../graph/Node.h"
#include "../../gpu/Texture.h"

namespace nf {

class LightShaftTexOp : public Node {
public:
    LightShaftTexOp(NodeId id, const std::string& name);
    ~LightShaftTexOp() override = default;

    bool Cook(const CookContext& context) override;

    glm::vec2 GetLightPosScreen() const;
    float GetDensity() const;
    float GetDecay() const;
    float GetWeight() const;
    int32_t GetSamples() const;

private:
    Pin* m_inTexPin = nullptr;
    Pin* m_inOcclusionPin = nullptr;
    Pin* m_outTexPin = nullptr;
    std::shared_ptr<gpu::Texture2D> m_outTexture;
};

} // namespace nf
