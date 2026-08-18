#pragma once

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include <GLFW/glfw3.h>

namespace gpu {

class Device {
public:
    Device() = default;
    ~Device();

    // Disable copy/move
    Device(const Device&) = delete;
    Device& operator=(const Device&) = delete;

    bool Initialize(GLFWwindow* window);
    void Cleanup();

    VkInstance GetInstance() const { return m_instance; }
    VkPhysicalDevice GetPhysicalDevice() const { return m_physicalDevice; }
    VkDevice GetDevice() const { return m_device; }
    VmaAllocator GetAllocator() const { return m_allocator; }
    VkQueue GetGraphicsQueue() const { return m_graphicsQueue; }
    uint32_t GetGraphicsQueueIndex() const { return m_graphicsQueueIndex; }
    VkQueue GetPresentQueue() const { return m_presentQueue; }
    uint32_t GetPresentQueueIndex() const { return m_presentQueueIndex; }

private:
    VkInstance m_instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkDevice m_device = VK_NULL_HANDLE;
    VkSurfaceKHR m_surface = VK_NULL_HANDLE;
    VmaAllocator m_allocator = VK_NULL_HANDLE;

    VkQueue m_graphicsQueue = VK_NULL_HANDLE;
    uint32_t m_graphicsQueueIndex = 0;
    VkQueue m_presentQueue = VK_NULL_HANDLE;
    uint32_t m_presentQueueIndex = 0;
};

} // namespace gpu
