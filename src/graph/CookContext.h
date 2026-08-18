#pragma once

#include <cstdint>

namespace gpu {
    class Device;
}

namespace nf {

/// Contextual state supplied to operators during graph evaluation.
struct CookContext {
    uint64_t frameIndex = 0;
    double timeSeconds = 0.0;
    double deltaTimeSeconds = 1.0 / 60.0;
    gpu::Device* gpuDevice = nullptr;
};

} // namespace nf
