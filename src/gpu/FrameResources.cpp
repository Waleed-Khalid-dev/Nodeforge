#include "FrameResources.h"
#include "Device.h"
#include <spdlog/spdlog.h>

namespace gpu {

FrameResources::FrameResources(Device* device, uint32_t maxFramesInFlight)
    : m_device(device), m_maxFramesInFlight(maxFramesInFlight) {
}

FrameResources::~FrameResources() {
    Cleanup();
}

bool FrameResources::Build() {
    m_frames.resize(m_maxFramesInFlight);

    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = m_device->GetGraphicsQueueIndex();

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < m_maxFramesInFlight; i++) {
        if (vkCreateCommandPool(m_device->GetDevice(), &poolInfo, nullptr, &m_frames[i].commandPool) != VK_SUCCESS) {
            spdlog::error("Failed to create command pool");
            return false;
        }

        allocInfo.commandPool = m_frames[i].commandPool;

        if (vkAllocateCommandBuffers(m_device->GetDevice(), &allocInfo, &m_frames[i].commandBuffer) != VK_SUCCESS) {
            spdlog::error("Failed to allocate command buffers");
            return false;
        }

        if (vkCreateSemaphore(m_device->GetDevice(), &semaphoreInfo, nullptr, &m_frames[i].imageAvailableSemaphore) != VK_SUCCESS ||
            vkCreateSemaphore(m_device->GetDevice(), &semaphoreInfo, nullptr, &m_frames[i].renderFinishedSemaphore) != VK_SUCCESS ||
            vkCreateFence(m_device->GetDevice(), &fenceInfo, nullptr, &m_frames[i].inFlightFence) != VK_SUCCESS) {
            spdlog::error("Failed to create synchronization objects for a frame");
            return false;
        }
    }

    return true;
}

void FrameResources::Cleanup() {
    VkDevice device = m_device->GetDevice();
    if (!device) return;

    for (size_t i = 0; i < m_frames.size(); i++) {
        if (m_frames[i].renderFinishedSemaphore) {
            vkDestroySemaphore(device, m_frames[i].renderFinishedSemaphore, nullptr);
        }
        if (m_frames[i].imageAvailableSemaphore) {
            vkDestroySemaphore(device, m_frames[i].imageAvailableSemaphore, nullptr);
        }
        if (m_frames[i].inFlightFence) {
            vkDestroyFence(device, m_frames[i].inFlightFence, nullptr);
        }
        if (m_frames[i].commandPool) {
            vkDestroyCommandPool(device, m_frames[i].commandPool, nullptr);
        }
    }
    m_frames.clear();
}

} // namespace gpu
