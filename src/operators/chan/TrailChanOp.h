#pragma once

#include "ChanOp.h"
#include <deque>
#include <unordered_map>

namespace nf {

class TrailChanOp : public ChanOp {
public:
    TrailChanOp(NodeId id, const std::string& name);
    bool Cook(const CookContext& context) override;

private:
    std::unordered_map<std::string, std::deque<float>> m_history;
};

} // namespace nf
