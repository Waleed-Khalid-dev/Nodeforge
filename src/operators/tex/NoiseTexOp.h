#pragma once

#include "TexOp.h"
#include "../../render/ComputePass.h"

namespace nf {

class NoiseTexOp : public TexOp {
public:
    NoiseTexOp(NodeId id, const std::string& name);
    virtual bool Cook(const CookContext& context) override;

private:
    std::unique_ptr<render::ComputePass> m_computePass;
};

} // namespace nf
