#pragma once

#include "../../graph/Node.h"
#include "../../gpu/Texture.h"

namespace nf {

class OpenXRHeadsetTexOp : public Node {
public:
    OpenXRHeadsetTexOp(NodeId id, const std::string& name);
    ~OpenXRHeadsetTexOp() override = default;

    bool Cook(const CookContext& context) override;

    float GetIPD() const;
    bool IsMirrorWindowEnabled() const;

private:
    Pin* m_inLeftEyePin = nullptr;
    Pin* m_inRightEyePin = nullptr;
    Pin* m_outPreviewPin = nullptr;
    std::shared_ptr<gpu::Texture2D> m_outTexture;
};

} // namespace nf
