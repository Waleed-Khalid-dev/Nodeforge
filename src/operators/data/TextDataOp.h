#pragma once

#include "DataOp.h"

namespace nf {

class TextDataOp : public DataOp {
public:
    TextDataOp(NodeId id, const std::string& name);
    bool Cook(const CookContext& context) override;
};

} // namespace nf
