#pragma once

#include "GeomOp.h"

namespace nf {

class SphereGeomOp : public GeomOp {
public:
    SphereGeomOp(NodeId id, const std::string& name);
    bool Cook(const CookContext& context) override;
};

} // namespace nf
