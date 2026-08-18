#pragma once

#include "../../graph/Node.h"
#include "../../gpu/Texture.h"

namespace nf {

class TransformTexOp : public Node {
public:
    TransformTexOp(NodeId id, const std::string& name);

    bool Cook(const CookContext& context) override;

private:
    Pin* m_inPin = nullptr;
    Pin* m_outPin = nullptr;
};

} // namespace nf
