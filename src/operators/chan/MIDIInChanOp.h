#pragma once

#include "ChanOp.h"

namespace nf {

class MIDIInChanOp : public ChanOp {
public:
    MIDIInChanOp(NodeId id, const std::string& name);
    ~MIDIInChanOp() override;

    bool Cook(const CookContext& context) override;

private:
    int32_t m_currentDevice{-1};
};

} // namespace nf
