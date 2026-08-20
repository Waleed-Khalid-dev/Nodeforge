#pragma once

#include <vector>
#include <mutex>
#include <memory>
#include <cstdint>
#include <algorithm>

namespace nf {

struct VideoFrame {
    uint32_t width = 0;
    uint32_t height = 0;
    double ptsSeconds = 0.0;
    uint64_t frameIndex = 0;
    std::vector<uint8_t> rgbaPixels;
};

class FrameRingBuffer {
public:
    explicit FrameRingBuffer(size_t capacity = 16)
        : m_capacity(std::max<size_t>(2, capacity)) {
    }

    void Push(VideoFrame&& frame) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_buffer.size() >= m_capacity) {
            m_buffer.erase(m_buffer.begin());
        }
        m_buffer.push_back(std::move(frame));
    }

    bool GetNearestFrame(double targetPts, VideoFrame& outFrame) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_buffer.empty()) return false;

        size_t bestIdx = 0;
        double bestDiff = std::abs(m_buffer[0].ptsSeconds - targetPts);

        for (size_t i = 1; i < m_buffer.size(); ++i) {
            double diff = std::abs(m_buffer[i].ptsSeconds - targetPts);
            if (diff < bestDiff) {
                bestDiff = diff;
                bestIdx = i;
            }
        }

        outFrame = m_buffer[bestIdx];
        return true;
    }

    bool PopOldest(VideoFrame& outFrame) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_buffer.empty()) return false;
        outFrame = std::move(m_buffer.front());
        m_buffer.erase(m_buffer.begin());
        return true;
    }

    void Clear() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_buffer.clear();
    }

    size_t Size() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_buffer.size();
    }

    bool IsEmpty() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_buffer.empty();
    }

private:
    mutable std::mutex m_mutex;
    size_t m_capacity;
    std::vector<VideoFrame> m_buffer;
};

} // namespace nf
