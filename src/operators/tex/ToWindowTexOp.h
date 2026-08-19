#pragma once

#include "TexOp.h"

namespace nf {

class ToWindowTexOp : public TexOp {
public:
    ToWindowTexOp(NodeId id, const std::string& name);
    virtual bool Cook(const CookContext& context) override;

private:
    Pin* m_inPin = nullptr;
};

} // namespace nf
