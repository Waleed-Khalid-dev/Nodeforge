#pragma once

#include "ChanOp.h"

namespace nf {

class DMXInChanOp : public ChanOp {
public:
    DMXInChanOp(NodeId id, const std::string& name);
    ~DMXInChanOp() override;

    bool Cook(const CookContext& context) override;
};

} // namespace nf
