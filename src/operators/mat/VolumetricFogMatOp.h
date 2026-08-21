#pragma once

#include "../../graph/Node.h"

namespace nf {

class VolumetricFogMatOp : public Node {
public:
    VolumetricFogMatOp(NodeId id, const std::string& name);
    ~VolumetricFogMatOp() override = default;

    bool Cook(const CookContext& context) override;

    float GetDensityScale() const;
    glm::vec4 GetFogColor() const;
    float GetNoiseFrequency() const;
    float GetAbsorption() const;

private:
    Pin* m_outMatPin = nullptr;
};

} // namespace nf
