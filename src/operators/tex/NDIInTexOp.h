#pragma once

#include "TexOp.h"

namespace nf {

class NDIInTexOp : public TexOp {
public:
    NDIInTexOp(NodeId id, const std::string& name);
    bool Cook(const CookContext& context) override;
};

} // namespace nf
