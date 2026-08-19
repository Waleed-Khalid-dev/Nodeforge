#pragma once

#include "ChanOp.h"

namespace nf {

class MergeChanOp : public ChanOp {
public:
    MergeChanOp(NodeId id, const std::string& name);
    bool Cook(const CookContext& context) override;
};

} // namespace nf
