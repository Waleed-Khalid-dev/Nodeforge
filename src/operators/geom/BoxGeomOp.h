#pragma once

#include "GeomOp.h"

namespace nf {

class BoxGeomOp : public GeomOp {
public:
    BoxGeomOp(NodeId id, const std::string& name);
    bool Cook(const CookContext& context) override;
};

} // namespace nf
