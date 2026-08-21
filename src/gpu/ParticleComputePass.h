#pragma once

#include <memory>
#include <string>
#include "ParticleBuffer.h"

namespace gpu {

class Device;

class ParticleComputePass {
public:
    ParticleComputePass(Device* device = nullptr);
    ~ParticleComputePass();

    bool Initialize(std::string* outError = nullptr);
    void Cleanup();

    void Dispatch(ParticleBuffer& buffer, const ParticleSimParams& params);

private:
    Device* m_device = nullptr;
    bool m_initialized = false;
};

} // namespace gpu
