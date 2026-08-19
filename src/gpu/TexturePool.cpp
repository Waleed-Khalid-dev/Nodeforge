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

std::shared_ptr<Texture2D> TexturePool::Acquire(uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage, uint64_t currentFrame) {
    std::lock_guard<std::mutex> lock(m_mutex);

    // Search available pool for exact match
    for (auto it = m_available.begin(); it != m_available.end(); ++it) {
        if (it->texture &&
            it->texture->GetWidth() == width &&
            it->texture->GetHeight() == height &&
            it->texture->GetFormat() == format &&
            (it->texture->GetUsage() & usage) == usage) {
            
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
    m_available.push_back({ std::move(texture), frameIndex });
}

void TexturePool::GarbageCollect(uint64_t currentFrame, uint64_t maxAgeFrames) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_available.erase(
        std::remove_if(m_available.begin(), m_available.end(), [currentFrame, maxAgeFrames](const TexturePoolEntry& entry) {
            return (currentFrame >= entry.lastUsedFrame) && ((currentFrame - entry.lastUsedFrame) > maxAgeFrames);
        }),
        m_available.end()
    );
}

void TexturePool::Clear() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_available.clear();
    m_totalCreated = 0;
}

size_t TexturePool::GetAvailableCount() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_available.size();
}

size_t TexturePool::GetTotalAllocatedCount() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_totalCreated;
}

} // namespace gpu
