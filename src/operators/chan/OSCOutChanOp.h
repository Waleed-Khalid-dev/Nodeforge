#pragma once

#include "ChanOp.h"
#include <unordered_map>
#include <string>

namespace nf {

class OSCOutChanOp : public ChanOp {
public:
    OSCOutChanOp(NodeId id, const std::string& name);
    ~OSCOutChanOp() override;

    bool Cook(const CookContext& context) override;

private:
    void SendOSCMessage(const std::string& host, int port, const std::string& address, float value);

    std::unordered_map<std::string, float> m_prevValues;
};

} // namespace nf
