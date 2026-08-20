#pragma once

#include "TexOp.h"

namespace nf {

class SpoutOutTexOp : public TexOp {
public:
    SpoutOutTexOp(NodeId id, const std::string& name);
    bool Cook(const CookContext& context) override;

private:
    Pin* m_inPin = nullptr;
};

} // namespace nf
