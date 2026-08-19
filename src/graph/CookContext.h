#pragma once

#include <cstdint>
#include <vulkan/vulkan.h>

namespace gpu {
    class Device;
    class TexturePool;
}

namespace nf {

/// Contextual state supplied to operators during graph evaluation.
struct CookContext {
    uint64_t frameIndex = 0;
    double timeSeconds = 0.0;
    double deltaTimeSeconds = 1.0 / 60.0;
    gpu::Device* gpuDevice = nullptr;
    gpu::TexturePool* texturePool = nullptr;
    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
};

} // namespace nf
