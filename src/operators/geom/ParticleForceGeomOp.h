#pragma once

#include "GeomOp.h"

namespace nf {

class ParticleForceGeomOp : public GeomOp {
public:
    ParticleForceGeomOp(NodeId id, const std::string& name);
    ~ParticleForceGeomOp() override = default;

    bool Cook(const CookContext& context) override;

private:
    Pin* m_inGeomPin = nullptr;
    Pin* m_inForceModPin = nullptr;
};

} // namespace nf
