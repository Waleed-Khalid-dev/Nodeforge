#pragma once

#include "ChanOp.h"

namespace nf {

class SelectChanOp : public ChanOp {
public:
    SelectChanOp(NodeId id, const std::string& name);
    bool Cook(const CookContext& context) override;
};

} // namespace nf
