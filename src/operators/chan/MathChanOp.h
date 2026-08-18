#pragma once

#include "../../graph/Node.h"

namespace nf {

class MathChanOp : public Node {
public:
    MathChanOp(NodeId id, const std::string& name);

    bool Cook(const CookContext& context) override;

private:
    Pin* m_inPin = nullptr;
    Pin* m_outPin = nullptr;
};

} // namespace nf
