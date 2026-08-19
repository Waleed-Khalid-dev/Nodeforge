#pragma once

#include "ChanOp.h"

namespace nf {

class LFOChanOp : public ChanOp {
public:
    LFOChanOp(NodeId id, const std::string& name);
    bool Cook(const CookContext& context) override;

private:
    double m_phaseAccum = 0.0;
};

} // namespace nf
