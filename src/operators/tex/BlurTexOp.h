#pragma once

#include "TexOp.h"
#include "../../render/FullscreenPass.h"

namespace nf {

class BlurTexOp : public TexOp {
public:
    BlurTexOp(NodeId id, const std::string& name);
    virtual bool Cook(const CookContext& context) override;

private:
    Pin* m_inPin = nullptr;
    std::unique_ptr<render::FullscreenPass> m_pass;
};

} // namespace nf
