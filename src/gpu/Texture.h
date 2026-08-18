#pragma once

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include <memory>

namespace gpu {

class Device;

class Texture2D {
public:
    Texture2D(Device* device);
    ~Texture2D();

    // Disable copy/move
    Texture2D(const Texture2D&) = delete;
    Texture2D& operator=(const Texture2D&) = delete;

    // Procedural checkerboard generation
    bool CreateProceduralCheckerboard();
    void Cleanup();

    VkImage GetImage() const { return m_image; }
    VkImageView GetImageView() const { return m_imageView; }
    VkSampler GetSampler() const { return m_sampler; }

private:
    bool UploadData(const void* pixelData, uint32_t width, uint32_t height, VkDeviceSize imageSize);

    Device* m_device = nullptr;
    VkImage m_image = VK_NULL_HANDLE;
    VmaAllocation m_allocation = VK_NULL_HANDLE;
    VkImageView m_imageView = VK_NULL_HANDLE;
    VkSampler m_sampler = VK_NULL_HANDLE;

    uint32_t m_width = 0;
    uint32_t m_height = 0;
};

} // namespace gpu
