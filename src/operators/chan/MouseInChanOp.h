#pragma once

#include "ChanOp.h"

namespace nf {

class MouseInChanOp : public ChanOp {
public:
    MouseInChanOp(NodeId id, const std::string& name);
    ~MouseInChanOp() override;

    bool Cook(const CookContext& context) override;
};

} // namespace nf
