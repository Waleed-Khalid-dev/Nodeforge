#pragma once

#include "../tex/TexOp.h"

namespace nf {

class ChanToTexOp : public TexOp {
public:
    ChanToTexOp(NodeId id, const std::string& name);
    bool Cook(const CookContext& context) override;

private:
    Pin* m_inPin = nullptr;
};

} // namespace nf
