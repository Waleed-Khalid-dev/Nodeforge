#pragma once

#include "ChanOp.h"

namespace nf {

class TimeChanOp : public ChanOp {
public:
    TimeChanOp(NodeId id, const std::string& name);
    bool Cook(const CookContext& context) override;
};

} // namespace nf
