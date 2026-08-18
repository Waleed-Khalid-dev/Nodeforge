#pragma once

#include "../../graph/Node.h"
#include "../../gpu/Texture.h"

namespace nf {

class ConstantTexOp : public Node {
public:
    ConstantTexOp(NodeId id, const std::string& name);

    bool Cook(const CookContext& context) override;

private:
    Pin* m_outPin = nullptr;
    std::shared_ptr<gpu::Texture2D> m_texture;
};

} // namespace nf
