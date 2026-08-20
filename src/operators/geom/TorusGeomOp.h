#pragma once

#include "GeomOp.h"

namespace nf {

class TorusGeomOp : public GeomOp {
public:
    TorusGeomOp(NodeId id, const std::string& name);
    bool Cook(const CookContext& context) override;
};

} // namespace nf
