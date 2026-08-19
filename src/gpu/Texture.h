#pragma once

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include <memory>
#include <vector>

namespace gpu {

class Device;

class Texture2D {
public:
    Texture2D(Device* device);
    ~Texture2D();

    // Disable copy/move
    Texture2D(const Texture2D&) = delete;
    Texture2D& operator=(const Texture2D&) = delete;

    bool Create(uint32_t width, uint32_t height, 
                VkFormat format = VK_FORMAT_R8G8B8A8_UNORM, 
                VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
                VkFilter filter = VK_FILTER_LINEAR,
                VkSamplerAddressMode wrapMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);

    // Procedural checkerboard generation
    bool CreateProceduralCheckerboard();
    bool UploadData(const void* pixelData, uint32_t width, uint32_t height, VkDeviceSize imageSize, VkFormat format = VK_FORMAT_R8G8B8A8_UNORM);
    bool ReadbackPixels(std::vector<uint8_t>& outPixels);
    void Cleanup();

    void TransitionLayout(VkCommandBuffer cmd, VkImageLayout newLayout,
                          VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage,
                          VkAccessFlags srcAccess, VkAccessFlags dstAccess);

    uint32_t GetWidth() const { return m_width; }
    uint32_t GetHeight() const { return m_height; }
    VkFormat GetFormat() const { return m_format; }
    VkImageUsageFlags GetUsage() const { return m_usage; }
    VkImage GetImage() const { return m_image; }
    VkImageView GetImageView() const { return m_imageView; }
    VkSampler GetSampler() const { return m_sampler; }
    VkImageLayout GetCurrentLayout() const { return m_currentLayout; }
    void SetCurrentLayout(VkImageLayout layout) { m_currentLayout = layout; }
    Device* GetDevice() const { return m_device; }

private:
    Device* m_device = nullptr;
    VkImage m_image = VK_NULL_HANDLE;
    VmaAllocation m_allocation = VK_NULL_HANDLE;
    VkImageView m_imageView = VK_NULL_HANDLE;
    VkSampler m_sampler = VK_NULL_HANDLE;

    uint32_t m_width = 0;
    uint32_t m_height = 0;
    VkFormat m_format = VK_FORMAT_R8G8B8A8_UNORM;
    VkImageUsageFlags m_usage = 0;
    VkImageLayout m_currentLayout = VK_IMAGE_LAYOUT_UNDEFINED;
};

} // namespace gpu
