#pragma once

#include "ChanOp.h"
#include <unordered_map>

namespace nf {

class FilterChanOp : public ChanOp {
public:
    FilterChanOp(NodeId id, const std::string& name);
    bool Cook(const CookContext& context) override;

private:
    struct ChannelState {
        float value = 0.0f;
        float velocity = 0.0f;
        bool initialized = false;
    };

    std::unordered_map<std::string, ChannelState> m_states;
};

} // namespace nf
