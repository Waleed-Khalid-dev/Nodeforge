#pragma once

#include "TexOp.h"

namespace nf {

class SpoutInTexOp : public TexOp {
public:
    SpoutInTexOp(NodeId id, const std::string& name);
    bool Cook(const CookContext& context) override;
};

} // namespace nf
