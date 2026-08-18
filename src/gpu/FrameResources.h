#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace gpu {

class Device;

struct FrameData {
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;
};

class FrameResources {
public:
    FrameResources(Device* device, uint32_t maxFramesInFlight = 2);
    ~FrameResources();

    // Disable copy/move
    FrameResources(const FrameResources&) = delete;
    FrameResources& operator=(const FrameResources&) = delete;

    bool Build();
    void Cleanup();

    const FrameData& GetCurrentFrame() const { return m_frames[m_currentFrame]; }
    void AdvanceFrame() { m_currentFrame = (m_currentFrame + 1) % m_maxFramesInFlight; }
    uint32_t GetCurrentFrameIndex() const { return m_currentFrame; }

private:
    Device* m_device = nullptr;
    uint32_t m_maxFramesInFlight = 2;
    uint32_t m_currentFrame = 0;

    std::vector<FrameData> m_frames;
};

} // namespace gpu
