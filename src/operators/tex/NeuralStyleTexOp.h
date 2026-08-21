#pragma once

#include "../../graph/Node.h"
#include "../../gpu/Texture.h"

namespace nf {

class NeuralStyleTexOp : public Node {
public:
    NeuralStyleTexOp(NodeId id, const std::string& name);
    ~NeuralStyleTexOp() override = default;

    bool Cook(const CookContext& context) override;

    int32_t GetStylePreset() const;
    float GetStyleWeight() const;
    bool GetPreserveColors() const;

private:
    Pin* m_inTexPin = nullptr;
    Pin* m_outTexPin = nullptr;
    std::shared_ptr<gpu::Texture2D> m_outTexture;
};

} // namespace nf
