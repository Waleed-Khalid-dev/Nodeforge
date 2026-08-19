#pragma once

#include "ChanOp.h"

namespace nf {

class NoiseChanOp : public ChanOp {
public:
    NoiseChanOp(NodeId id, const std::string& name);
    bool Cook(const CookContext& context) override;

private:
    static float GradientNoise1D(float x, int32_t seed);
    static float FBM1D(float x, int32_t octaves, float roughness, int32_t seed);
};

} // namespace nf
