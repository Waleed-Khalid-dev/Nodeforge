#pragma once

#include "../../graph/Node.h"
#include "../../gpu/Texture.h"

namespace nf {

class SegmentationMaskTexOp : public Node {
public:
    SegmentationMaskTexOp(NodeId id, const std::string& name);
    ~SegmentationMaskTexOp() override = default;

    bool Cook(const CookContext& context) override;

    float GetThreshold() const;
    float GetFeather() const;

private:
    Pin* m_inTexPin = nullptr;
    Pin* m_outTexPin = nullptr;
    Pin* m_outMaskPin = nullptr;
    std::shared_ptr<gpu::Texture2D> m_outTexture;
    std::shared_ptr<gpu::Texture2D> m_outMask;
};

} // namespace nf
