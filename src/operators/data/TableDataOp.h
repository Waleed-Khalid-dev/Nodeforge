#pragma once

#include "DataOp.h"

namespace nf {

class TableDataOp : public DataOp {
public:
    TableDataOp(NodeId id, const std::string& name);
    bool Cook(const CookContext& context) override;
};

} // namespace nf
