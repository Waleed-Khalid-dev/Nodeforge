#pragma once

#include "DataOp.h"

namespace nf {

class ScriptDataOp : public DataOp {
public:
    ScriptDataOp(NodeId id, const std::string& name);
    bool Cook(const CookContext& context) override;
    void Pulse(const std::string& paramName);

private:
    Pin* m_inPin = nullptr;
};

} // namespace nf
