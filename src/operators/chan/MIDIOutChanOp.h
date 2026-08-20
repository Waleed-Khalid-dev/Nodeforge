#pragma once

#include "ChanOp.h"
#include <unordered_map>

namespace nf {

class MIDIOutChanOp : public ChanOp {
public:
    MIDIOutChanOp(NodeId id, const std::string& name);
    ~MIDIOutChanOp() override;

    bool Cook(const CookContext& context) override;

private:
    int32_t m_currentDevice{-1};
    std::unordered_map<std::string, float> m_prevValues;
    std::unordered_map<std::string, bool> m_noteState;
};

} // namespace nf
