#include "Texture.h"
#include "Device.h"
#include <spdlog/spdlog.h>
#include <vector>
#include <cstring>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace gpu {

Texture2D::Texture2D(Device* device) : m_device(device) {
}

Texture2D::~Texture2D() {
    Cleanup();
}

void Texture2D::Cleanup() {
    if (!m_device || !m_device->GetDevice()) return;

    if (m_sampler) {
        vkDestroySampler(m_device->GetDevice(), m_sampler, nullptr);
        m_sampler = VK_NULL_HANDLE;
    }
    if (m_imageView) {
        vkDestroyImageView(m_device->GetDevice(), m_imageView, nullptr);
        m_imageView = VK_NULL_HANDLE;
    }
    if (m_image) {
        vmaDestroyImage(m_device->GetAllocator(), m_image, m_allocation);
        m_image = VK_NULL_HANDLE;
        m_allocation = VK_NULL_HANDLE;
    }
    m_currentLayout = VK_IMAGE_LAYOUT_UNDEFINED;
}

bool Texture2D::Create(uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage, VkFilter filter, VkSamplerAddressMode wrapMode) {
    Cleanup();

    m_width = width;
    m_height = height;
    m_format = format;
    m_usage = usage;

    // Create Vulkan Image
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo imageAllocInfo = {};
    imageAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    if (vmaCreateImage(m_device->GetAllocator(), &imageInfo, &imageAllocInfo, &m_image, &m_allocation, nullptr) != VK_SUCCESS) {
        spdlog::error("Failed to allocate GPU texture image ({}x{})", width, height);
        return false;
    }

    // Create ImageView
    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = m_image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(m_device->GetDevice(), &viewInfo, nullptr, &m_imageView) != VK_SUCCESS) {
        spdlog::error("Failed to create texture image view");
        return false;
    }

    // Create Sampler
    VkSamplerCreateInfo samplerInfo = {};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = filter;
    samplerInfo.minFilter = filter;
    samplerInfo.addressModeU = wrapMode;
    samplerInfo.addressModeV = wrapMode;
    samplerInfo.addressModeW = wrapMode;
    samplerInfo.anisotropyEnable = VK_FALSE;
    samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

    if (vkCreateSampler(m_device->GetDevice(), &samplerInfo, nullptr, &m_sampler) != VK_SUCCESS) {
        spdlog::error("Failed to create texture sampler");
        return false;
    }

    m_currentLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    return true;
}

bool Texture2D::CreateProceduralCheckerboard() {
    m_width = 256;
    m_height = 256;
    VkDeviceSize imageSize = m_width * m_height * 4;

    std::vector<uint8_t> pixels(imageSize);
    for (uint32_t y = 0; y < m_height; ++y) {
        for (uint32_t x = 0; x < m_width; ++x) {
            uint8_t color = ((x / 32) % 2 == (y / 32) % 2) ? 255 : 64;
            uint32_t index = (y * m_width + x) * 4;
            pixels[index + 0] = color;
            pixels[index + 1] = color;
            pixels[index + 2] = color;
            pixels[index + 3] = 255;
        }
    }

    return UploadData(pixels.data(), m_width, m_height, imageSize, VK_FORMAT_R8G8B8A8_UNORM);
}

bool Texture2D::UploadData(const void* pixelData, uint32_t width, uint32_t height, VkDeviceSize imageSize, VkFormat format) {
    if (!Create(width, height, format, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT)) {
        return false;
    }

    // Create staging buffer
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = imageSize;
    bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

    VkBuffer stagingBuffer;
    VmaAllocation stagingAllocation;
    if (vmaCreateBuffer(m_device->GetAllocator(), &bufferInfo, &allocInfo, &stagingBuffer, &stagingAllocation, nullptr) != VK_SUCCESS) {
        spdlog::error("Failed to create staging buffer");
        return false;
    }

    void* data;
    vmaMapMemory(m_device->GetAllocator(), stagingAllocation, &data);
    std::memcpy(data, pixelData, static_cast<size_t>(imageSize));
    vmaUnmapMemory(m_device->GetAllocator(), stagingAllocation);

    // Command Buffer for Transfer
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
    poolInfo.queueFamilyIndex = m_device->GetGraphicsQueueIndex();

    VkCommandPool commandPool;
    vkCreateCommandPool(m_device->GetDevice(), &poolInfo, nullptr, &commandPool);

    VkCommandBufferAllocateInfo allocCmdInfo = {};
    allocCmdInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocCmdInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocCmdInfo.commandPool = commandPool;
    allocCmdInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(m_device->GetDevice(), &allocCmdInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    // Transition to TRANSFER_DST_OPTIMAL
    TransitionLayout(commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                     VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
                     0, VK_ACCESS_TRANSFER_WRITE_BIT);

    // Copy buffer to image
    VkBufferImageCopy region = {};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = {0, 0, 0};
    region.imageExtent = {width, height, 1};

    vkCmdCopyBufferToImage(commandBuffer, stagingBuffer, m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    // Transition to SHADER_READ_ONLY_OPTIMAL
    TransitionLayout(commandBuffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                     VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                     VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(m_device->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_device->GetGraphicsQueue());

    vkFreeCommandBuffers(m_device->GetDevice(), commandPool, 1, &commandBuffer);
    vkDestroyCommandPool(m_device->GetDevice(), commandPool, nullptr);
    vmaDestroyBuffer(m_device->GetAllocator(), stagingBuffer, stagingAllocation);

    return true;
}

void Texture2D::TransitionLayout(VkCommandBuffer cmd, VkImageLayout newLayout,
                                VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage,
                                VkAccessFlags srcAccess, VkAccessFlags dstAccess) {
    if (m_currentLayout == newLayout) return;

    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = m_currentLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = m_image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.srcAccessMask = srcAccess;
    barrier.dstAccessMask = dstAccess;

    vkCmdPipelineBarrier(cmd, srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    m_currentLayout = newLayout;
}

bool Texture2D::ReadbackPixels(std::vector<uint8_t>& outPixels) {
    if (!m_image || m_width == 0 || m_height == 0) return false;

    VkDeviceSize imageSize = m_width * m_height * 4;
    outPixels.resize(imageSize);

    // Create staging destination buffer
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = imageSize;
    bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

    VkBuffer stagingBuffer;
    VmaAllocation stagingAllocation;
    if (vmaCreateBuffer(m_device->GetAllocator(), &bufferInfo, &allocInfo, &stagingBuffer, &stagingAllocation, nullptr) != VK_SUCCESS) {
        spdlog::error("Failed to create readback staging buffer");
        return false;
    }

    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
    poolInfo.queueFamilyIndex = m_device->GetGraphicsQueueIndex();

    VkCommandPool commandPool;
    vkCreateCommandPool(m_device->GetDevice(), &poolInfo, nullptr, &commandPool);

    VkCommandBufferAllocateInfo allocCmdInfo = {};
    allocCmdInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocCmdInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocCmdInfo.commandPool = commandPool;
    allocCmdInfo.commandBufferCount = 1;

    VkCommandBuffer cmd;
    vkAllocateCommandBuffers(m_device->GetDevice(), &allocCmdInfo, &cmd);

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(cmd, &beginInfo);

    VkImageLayout prevLayout = m_currentLayout;
    TransitionLayout(cmd, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                     VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
                     VK_ACCESS_MEMORY_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                     VK_ACCESS_TRANSFER_READ_BIT);

    VkBufferImageCopy region = {};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = {0, 0, 0};
    region.imageExtent = {m_width, m_height, 1};

    vkCmdCopyImageToBuffer(cmd, m_image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, stagingBuffer, 1, &region);

    if (prevLayout != VK_IMAGE_LAYOUT_UNDEFINED) {
        TransitionLayout(cmd, prevLayout,
                         VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                         VK_ACCESS_TRANSFER_READ_BIT, VK_ACCESS_SHADER_READ_BIT);
    }

    vkEndCommandBuffer(cmd);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmd;

    vkQueueSubmit(m_device->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_device->GetGraphicsQueue());

    vkFreeCommandBuffers(m_device->GetDevice(), commandPool, 1, &cmd);
    vkDestroyCommandPool(m_device->GetDevice(), commandPool, nullptr);

    void* mapped;
    vmaMapMemory(m_device->GetAllocator(), stagingAllocation, &mapped);
    std::memcpy(outPixels.data(), mapped, static_cast<size_t>(imageSize));
    vmaUnmapMemory(m_device->GetAllocator(), stagingAllocation);
    vmaDestroyBuffer(m_device->GetAllocator(), stagingBuffer, stagingAllocation);

    return true;
}

} // namespace gpu
