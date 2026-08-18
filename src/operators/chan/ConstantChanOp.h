#pragma once

#include "../../graph/Node.h"

namespace nf {

class ConstantChanOp : public Node {
public:
    ConstantChanOp(NodeId id, const std::string& name);

    bool Cook(const CookContext& context) override;

private:
    Pin* m_outPin = nullptr;
};

} // namespace nf
