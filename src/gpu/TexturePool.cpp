#include "TexturePool.h"
#include "Device.h"
#include <spdlog/spdlog.h>
#include <algorithm>

namespace gpu {

TexturePool::TexturePool(Device* device) : m_device(device) {
}

TexturePool::~TexturePool() {
    Clear();
}

static size_t CalculateBytes(uint32_t width, uint32_t height, VkFormat format) {
    size_t bpp = 4;
    switch (format) {
        case VK_FORMAT_R8G8B8A8_UNORM:
        case VK_FORMAT_B8G8R8A8_UNORM:
        case VK_FORMAT_R8G8B8A8_SRGB:
            bpp = 4; break;
        case VK_FORMAT_R16G16B16A16_SFLOAT:
            bpp = 8; break;
        case VK_FORMAT_R32G32B32A32_SFLOAT:
            bpp = 16; break;
        case VK_FORMAT_R8_UNORM:
            bpp = 1; break;
        case VK_FORMAT_R16_SFLOAT:
            bpp = 2; break;
        case VK_FORMAT_R32_SFLOAT:
            bpp = 4; break;
        default:
            bpp = 4; break;
    }
    return static_cast<size_t>(width) * height * bpp;
}

std::shared_ptr<Texture2D> TexturePool::Acquire(uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage, uint64_t currentFrame, const std::string& /*requesterName*/) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_totalAcquires++;

    // Search available pool for exact match
    for (auto it = m_available.begin(); it != m_available.end(); ++it) {
        if (it->texture &&
            it->texture->GetWidth() == width &&
            it->texture->GetHeight() == height &&
            it->texture->GetFormat() == format &&
            (it->texture->GetUsage() & usage) == usage) {
            
            m_poolHits++;
            m_activeLeases++;
            m_peakLeases = std::max(m_peakLeases, m_activeLeases);

            auto rawTex = it->texture.release();
            m_available.erase(it);

            // Wrap in custom shared_ptr deleter to auto-recycle upon destruction
            return std::shared_ptr<Texture2D>(rawTex, [this, currentFrame](Texture2D* ptr) {
                if (ptr) {
                    this->Recycle(std::unique_ptr<Texture2D>(ptr), currentFrame);
                }
            });
        }
    }

    // Allocate new texture
    auto newTex = std::make_unique<Texture2D>(m_device);
    if (!newTex->Create(width, height, format, usage)) {
        spdlog::error("TexturePool failed to create new texture ({}x{})", width, height);
        return nullptr;
    }

    m_totalCreated++;
    m_activeLeases++;
    m_peakLeases = std::max(m_peakLeases, m_activeLeases);
    m_estimatedVramBytes += CalculateBytes(width, height, format);

    auto rawTex = newTex.release();
    return std::shared_ptr<Texture2D>(rawTex, [this, currentFrame](Texture2D* ptr) {
        if (ptr) {
            this->Recycle(std::unique_ptr<Texture2D>(ptr), currentFrame);
        }
    });
}

void TexturePool::Recycle(std::unique_ptr<Texture2D> texture, uint64_t frameIndex) {
    if (!texture) return;
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_activeLeases > 0) m_activeLeases--;
    m_available.push_back({ std::move(texture), frameIndex });
}

void TexturePool::GarbageCollect(uint64_t currentFrame, uint64_t maxAgeFrames) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_available.erase(
        std::remove_if(m_available.begin(), m_available.end(), [this, currentFrame, maxAgeFrames](const TexturePoolEntry& entry) {
            bool remove = (currentFrame >= entry.lastUsedFrame) && ((currentFrame - entry.lastUsedFrame) > maxAgeFrames);
            if (remove && entry.texture) {
                size_t bytes = CalculateBytes(entry.texture->GetWidth(), entry.texture->GetHeight(), entry.texture->GetFormat());
                if (m_estimatedVramBytes >= bytes) m_estimatedVramBytes -= bytes;
            }
            return remove;
        }),
        m_available.end()
    );
}

void TexturePool::Clear() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_available.clear();
    m_totalCreated = 0;
    m_activeLeases = 0;
    m_estimatedVramBytes = 0;
}

size_t TexturePool::GetAvailableCount() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_available.size();
}

size_t TexturePool::GetTotalAllocatedCount() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_totalCreated;
}

size_t TexturePool::GetActiveLeasedCount() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_activeLeases;
}

size_t TexturePool::GetPeakLeasedCount() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_peakLeases;
}

size_t TexturePool::GetEstimatedVramBytes() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_estimatedVramBytes;
}

TexturePoolStats TexturePool::GetStats() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    TexturePoolStats stats;
    stats.activeLeases = m_activeLeases;
    stats.availableCount = m_available.size();
    stats.totalAllocated = m_totalCreated;
    stats.peakLeased = m_peakLeases;
    stats.totalAcquires = m_totalAcquires;
    stats.poolHits = m_poolHits;
    stats.estimatedVramBytes = m_estimatedVramBytes;
    stats.hitRate = (m_totalAcquires > 0) ? (static_cast<float>(m_poolHits) / m_totalAcquires) : 1.0f;
    return stats;
}

} // namespace gpu
