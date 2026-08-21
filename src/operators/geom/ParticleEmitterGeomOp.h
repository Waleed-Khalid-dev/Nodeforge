#pragma once

#include "GeomOp.h"
#include "../../gpu/ParticleBuffer.h"
#include <memory>

namespace nf {

class ParticleEmitterGeomOp : public GeomOp {
public:
    ParticleEmitterGeomOp(NodeId id, const std::string& name);
    ~ParticleEmitterGeomOp() override = default;

    bool Cook(const CookContext& context) override;

    gpu::ParticleBuffer& GetParticleBuffer() { return m_particleBuffer; }
    const gpu::ParticleBuffer& GetParticleBuffer() const { return m_particleBuffer; }

private:
    gpu::ParticleBuffer m_particleBuffer;
    Pin* m_inMeshPin = nullptr;
    Pin* m_inTriggerPin = nullptr;
};

} // namespace nf
