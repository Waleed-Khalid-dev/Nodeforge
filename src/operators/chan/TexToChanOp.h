#pragma once

#include "ChanOp.h"

namespace nf {

class TexToChanOp : public ChanOp {
public:
    TexToChanOp(NodeId id, const std::string& name);
    bool Cook(const CookContext& context) override;

private:
    Pin* m_inPin = nullptr;
};

} // namespace nf
