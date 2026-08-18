#include "Swapchain.h"
#include "Device.h"
#include <spdlog/spdlog.h>
#include <VkBootstrap.h>

namespace gpu {

Swapchain::Swapchain(Device* device) : m_device(device) {
}

Swapchain::~Swapchain() {
    Cleanup();
}

bool Swapchain::Build() {
    vkb::SwapchainBuilder swapchain_builder{m_device->GetPhysicalDevice(), m_device->GetDevice(), m_device->GetSurface()};

    auto vkb_swapchain_ret = swapchain_builder
                                 .use_default_format_selection()
                                 .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR) // V-Sync
                                 .build();

    if (!vkb_swapchain_ret) {
        spdlog::error("Failed to build swapchain: {}", vkb_swapchain_ret.error().message());
        return false;
    }

    vkb::Swapchain vkb_swapchain = vkb_swapchain_ret.value();
    m_swapchain = vkb_swapchain.swapchain;
    m_imageFormat = vkb_swapchain.image_format;
    m_extent = vkb_swapchain.extent;

    auto images_ret = vkb_swapchain.get_images();
    if (images_ret) {
        m_images = images_ret.value();
    } else {
        spdlog::error("Failed to get swapchain images: {}", images_ret.error().message());
        return false;
    }

    auto image_views_ret = vkb_swapchain.get_image_views();
    if (image_views_ret) {
        m_imageViews = image_views_ret.value();
    } else {
        spdlog::error("Failed to get swapchain image views: {}", image_views_ret.error().message());
        return false;
    }

    return true;
}

void Swapchain::Cleanup() {
    VkDevice device = m_device->GetDevice();
    if (!device) return;

    for (auto imageView : m_imageViews) {
        vkDestroyImageView(device, imageView, nullptr);
    }
    m_imageViews.clear();
    m_images.clear();

    if (m_swapchain) {
        vkDestroySwapchainKHR(device, m_swapchain, nullptr);
        m_swapchain = VK_NULL_HANDLE;
    }
}

bool Swapchain::Rebuild() {
    Cleanup();
    return Build();
}

} // namespace gpu
