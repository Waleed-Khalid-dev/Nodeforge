#pragma once

#include "GeomOp.h"

namespace nf {

class GridGeomOp : public GeomOp {
public:
    GridGeomOp(NodeId id, const std::string& name);
    bool Cook(const CookContext& context) override;
};

} // namespace nf
