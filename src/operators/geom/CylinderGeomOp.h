#pragma once

#include "GeomOp.h"

namespace nf {

class CylinderGeomOp : public GeomOp {
public:
    CylinderGeomOp(NodeId id, const std::string& name);
    bool Cook(const CookContext& context) override;
};

} // namespace nf
