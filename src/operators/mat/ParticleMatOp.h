#pragma once

#include "MatOp.h"

namespace nf {

class ParticleMatOp : public MatOp {
public:
    ParticleMatOp(NodeId id, const std::string& name);
    ~ParticleMatOp() override = default;

    bool Cook(const CookContext& context) override;

private:
    Pin* m_inTexturePin = nullptr;
};

} // namespace nf
