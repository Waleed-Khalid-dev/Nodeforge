#pragma once

#include "TexOp.h"
#include "../../render/FullscreenPass.h"

namespace nf {

class ConstantTexOp : public TexOp {
public:
    ConstantTexOp(NodeId id, const std::string& name);
    virtual bool Cook(const CookContext& context) override;

private:
    std::unique_ptr<render::FullscreenPass> m_pass;
};

} // namespace nf
