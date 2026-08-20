#pragma once

#include "ChanOp.h"
#include <vector>
#include <string>

namespace nf {

class KeyboardInChanOp : public ChanOp {
public:
    KeyboardInChanOp(NodeId id, const std::string& name);
    ~KeyboardInChanOp() override;

    bool Cook(const CookContext& context) override;

private:
    std::string m_cachedKeysStr;
    std::vector<std::string> m_cachedKeyList;
    std::vector<std::string> m_cachedChanNames;
};

} // namespace nf
