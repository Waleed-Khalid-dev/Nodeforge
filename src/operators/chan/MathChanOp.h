#pragma once

#include "ChanOp.h"

namespace nf {

class MathChanOp : public ChanOp {
public:
    MathChanOp(NodeId id, const std::string& name);
    bool Cook(const CookContext& context) override;

private:
    Pin* m_inPin1 = nullptr;
    Pin* m_inPin2 = nullptr;
};

} // namespace nf
