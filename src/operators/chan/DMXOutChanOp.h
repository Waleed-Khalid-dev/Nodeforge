#pragma once

#include "ChanOp.h"

namespace nf {

class DMXOutChanOp : public ChanOp {
public:
    DMXOutChanOp(NodeId id, const std::string& name);
    ~DMXOutChanOp() override;

    bool Cook(const CookContext& context) override;
};

} // namespace nf
