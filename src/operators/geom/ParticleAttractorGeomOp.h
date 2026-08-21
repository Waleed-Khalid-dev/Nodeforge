#pragma once

#include "GeomOp.h"

namespace nf {

class ParticleAttractorGeomOp : public GeomOp {
public:
    ParticleAttractorGeomOp(NodeId id, const std::string& name);
    ~ParticleAttractorGeomOp() override = default;

    bool Cook(const CookContext& context) override;

private:
    Pin* m_inGeomPin = nullptr;
    Pin* m_inTargetPin = nullptr;
};

} // namespace nf
