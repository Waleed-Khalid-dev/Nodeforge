#pragma once

#include "GeomOp.h"

namespace nf {

class NoiseDeformGeomOp : public GeomOp {
public:
    NoiseDeformGeomOp(NodeId id, const std::string& name);
    bool Cook(const CookContext& context) override;

private:
    Pin* m_inPin = nullptr;
};

} // namespace nf
