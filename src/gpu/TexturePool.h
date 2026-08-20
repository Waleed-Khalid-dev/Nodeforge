#pragma once

#include "Texture.h"
#include <vector>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <string>

namespace gpu {

class Device;

struct TexturePoolKey {
    uint32_t width = 0;
    uint32_t height = 0;
    VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
    VkImageUsageFlags usage = 0;

    bool operator==(const TexturePoolKey& other) const {
        return width == other.width &&
               height == other.height &&
               format == other.format &&
               usage == other.usage;
    }
};

struct TexturePoolEntry {
    std::unique_ptr<Texture2D> texture;
    uint64_t lastUsedFrame = 0;
};

struct TexturePoolStats {
    size_t activeLeases = 0;
    size_t availableCount = 0;
    size_t totalAllocated = 0;
    size_t peakLeased = 0;
    size_t totalAcquires = 0;
    size_t poolHits = 0;
    size_t estimatedVramBytes = 0;
    float hitRate = 0.0f;
};

class TexturePool {
public:
    TexturePool(Device* device);
    ~TexturePool();

    TexturePool(const TexturePool&) = delete;
    TexturePool& operator=(const TexturePool&) = delete;

    std::shared_ptr<Texture2D> Acquire(uint32_t width, uint32_t height,
                                       VkFormat format = VK_FORMAT_R8G8B8A8_UNORM,
                                       VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
                                       uint64_t currentFrame = 0,
                                       const std::string& requesterName = "Unknown");

    void GarbageCollect(uint64_t currentFrame, uint64_t maxAgeFrames = 120);
    void Clear();

    // Diagnostics & Watermark Queries
    size_t GetAvailableCount() const;
    size_t GetTotalAllocatedCount() const;
    size_t GetActiveLeasedCount() const;
    size_t GetPeakLeasedCount() const;
    size_t GetEstimatedVramBytes() const;
    TexturePoolStats GetStats() const;

private:
    void Recycle(std::unique_ptr<Texture2D> texture, uint64_t frameIndex);

    Device* m_device = nullptr;
    mutable std::mutex m_mutex;
    std::vector<TexturePoolEntry> m_available;
    size_t m_totalCreated = 0;
    size_t m_activeLeases = 0;
    size_t m_peakLeases = 0;
    size_t m_totalAcquires = 0;
    size_t m_poolHits = 0;
    size_t m_estimatedVramBytes = 0;
};

} // namespace gpu
