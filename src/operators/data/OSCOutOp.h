#pragma once

#include "DataOp.h"

namespace nf {

class OSCOutOp : public DataOp {
public:
    OSCOutOp(NodeId id, const std::string& name);
    bool Cook(const CookContext& context) override;
    void SendPacket(const std::string& address, const std::string& payload);

private:
    Pin* m_inPin = nullptr;
};

} // namespace nf
