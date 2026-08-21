#include "ParticleComputePass.h"
#include "Device.h"
#include "ShaderCompiler.h"
#include <spdlog/spdlog.h>

namespace gpu {

ParticleComputePass::ParticleComputePass(Device* device)
    : m_device(device) {
}

ParticleComputePass::~ParticleComputePass() {
    Cleanup();
}

bool ParticleComputePass::Initialize(std::string* /*outError*/) {
    m_initialized = true;
    return true;
}

void ParticleComputePass::Cleanup() {
    m_initialized = false;
}

void ParticleComputePass::Dispatch(ParticleBuffer& buffer, const ParticleSimParams& params) {
    // Step the simulation buffer
    buffer.StepSimulation(params);
}

} // namespace gpu
