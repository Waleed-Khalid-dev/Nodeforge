#pragma once

#include "TexOp.h"
#include "../../render/FullscreenPass.h"

namespace nf {

class CompositeTexOp : public TexOp {
public:
    CompositeTexOp(NodeId id, const std::string& name);
    virtual bool Cook(const CookContext& context) override;

private:
    Pin* m_inPin1 = nullptr;
    Pin* m_inPin2 = nullptr;
    std::unique_ptr<render::FullscreenPass> m_pass;
};

} // namespace nf
