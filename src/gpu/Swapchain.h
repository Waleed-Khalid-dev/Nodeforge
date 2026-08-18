#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace gpu {

class Device;

class Swapchain {
public:
    Swapchain(Device* device);
    ~Swapchain();

    // Disable copy/move
    Swapchain(const Swapchain&) = delete;
    Swapchain& operator=(const Swapchain&) = delete;

    bool Build();
    void Cleanup();
    bool Rebuild();

    VkSwapchainKHR GetSwapchain() const { return m_swapchain; }
    VkFormat GetImageFormat() const { return m_imageFormat; }
    const std::vector<VkImage>& GetImages() const { return m_images; }
    const std::vector<VkImageView>& GetImageViews() const { return m_imageViews; }
    VkExtent2D GetExtent() const { return m_extent; }

private:
    Device* m_device = nullptr;
    VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
    VkFormat m_imageFormat = VK_FORMAT_UNDEFINED;
    VkExtent2D m_extent = {0, 0};

    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_imageViews;
};

} // namespace gpu
