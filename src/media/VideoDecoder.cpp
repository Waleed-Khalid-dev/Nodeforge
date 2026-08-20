#include "VideoDecoder.h"
#include <chrono>
#include <cmath>
#include <spdlog/spdlog.h>

namespace nf {

VideoDecoder::VideoDecoder()
    : m_ringBuffer(32) {
    StartDecodeThread();
}

VideoDecoder::~VideoDecoder() {
    StopDecodeThread();
}

bool VideoDecoder::OpenFile(const std::string& filePath) {
    m_info.filePath = filePath;
    m_info.width = 1920;
    m_info.height = 1080;
    m_info.fps = 60.0;
    m_info.durationSeconds = 10.0;
    m_info.totalFrames = static_cast<uint64_t>(m_info.durationSeconds * m_info.fps);
    m_info.isLoaded = true;

    m_currentTime.store(0.0);
    m_isPingPongReverse.store(false);
    m_ringBuffer.Clear();

    return true;
}

void VideoDecoder::Close() {
    m_info.isLoaded = false;
    m_info.filePath.clear();
    m_ringBuffer.Clear();
}

void VideoDecoder::Play() {
    m_isPlaying.store(true);
}

void VideoDecoder::Pause() {
    m_isPlaying.store(false);
}

void VideoDecoder::SetSpeed(float speed) {
    m_speed.store(speed);
}

void VideoDecoder::SetLoopMode(VideoLoopMode mode) {
    m_loopMode.store(static_cast<int>(mode));
}

void VideoDecoder::Seek(double timeSeconds) {
    if (m_info.durationSeconds > 0.0) {
        timeSeconds = std::clamp(timeSeconds, 0.0, m_info.durationSeconds);
    }
    m_currentTime.store(timeSeconds);
    m_ringBuffer.Clear();
}

void VideoDecoder::SeekFrame(uint64_t frameIndex) {
    if (m_info.fps > 0.0) {
        Seek(static_cast<double>(frameIndex) / m_info.fps);
    }
}

bool VideoDecoder::GetFrame(double /*currentTime*/, VideoFrame& outFrame) {
    if (m_ringBuffer.PopOldest(outFrame)) {
        return true;
    }
    // Fallback on demand
    double t = m_currentTime.load();
    uint64_t idx = static_cast<uint64_t>(t * m_info.fps);
    outFrame = GenerateProceduralFrame(t, idx);
    return true;
}

void VideoDecoder::StartDecodeThread() {
    m_shouldStop.store(false);
    m_decodeThread = std::thread(&VideoDecoder::DecodeLoop, this);
}

void VideoDecoder::StopDecodeThread() {
    m_shouldStop.store(true);
    if (m_decodeThread.joinable()) {
        m_decodeThread.join();
    }
}

void VideoDecoder::DecodeLoop() {
    while (!m_shouldStop.load()) {
        if (!m_isPlaying.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        double dt = (1.0 / std::max(1.0, m_info.fps)) * m_speed.load();
        double curr = m_currentTime.load();
        int mode = m_loopMode.load();

        if (mode == static_cast<int>(VideoLoopMode::PingPong)) {
            if (m_isPingPongReverse.load()) {
                curr -= std::abs(dt);
                if (curr <= 0.0) {
                    curr = 0.0;
                    m_isPingPongReverse.store(false);
                }
            } else {
                curr += std::abs(dt);
                if (curr >= m_info.durationSeconds) {
                    curr = m_info.durationSeconds;
                    m_isPingPongReverse.store(true);
                }
            }
        } else if (mode == static_cast<int>(VideoLoopMode::Once)) {
            curr += dt;
            if (curr >= m_info.durationSeconds) {
                curr = m_info.durationSeconds;
                m_isPlaying.store(false);
            }
        } else { // Loop
            curr += dt;
            if (curr >= m_info.durationSeconds) {
                curr = std::fmod(curr, m_info.durationSeconds);
            } else if (curr < 0.0) {
                curr = m_info.durationSeconds - std::fmod(-curr, m_info.durationSeconds);
            }
        }

        m_currentTime.store(curr);
        uint64_t fIdx = static_cast<uint64_t>(curr * m_info.fps);

        if (m_ringBuffer.Size() < 16) {
            VideoFrame frame = GenerateProceduralFrame(curr, fIdx);
            m_ringBuffer.Push(std::move(frame));
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

VideoFrame VideoDecoder::GenerateProceduralFrame(double timestamp, uint64_t frameIndex) {
    VideoFrame frame;
    frame.width = m_info.width;
    frame.height = m_info.height;
    frame.ptsSeconds = timestamp;
    frame.frameIndex = frameIndex;

    // Small low-overhead pattern generator for real-time tests
    uint32_t w = std::min(frame.width, 320u);
    uint32_t h = std::min(frame.height, 180u);
    frame.width = w;
    frame.height = h;
    frame.rgbaPixels.resize(w * h * 4);

    float t = static_cast<float>(timestamp);
    uint8_t rVal = static_cast<uint8_t>((std::sin(t * 2.0f) * 0.5f + 0.5f) * 255.0f);
    uint8_t gVal = static_cast<uint8_t>((std::cos(t * 3.0f) * 0.5f + 0.5f) * 255.0f);
    uint8_t bVal = static_cast<uint8_t>((std::sin(t * 1.5f + 1.0f) * 0.5f + 0.5f) * 255.0f);

    for (size_t i = 0; i < frame.rgbaPixels.size(); i += 4) {
        frame.rgbaPixels[i + 0] = rVal;
        frame.rgbaPixels[i + 1] = gVal;
        frame.rgbaPixels[i + 2] = bVal;
        frame.rgbaPixels[i + 3] = 255;
    }

    return frame;
}

} // namespace nf
