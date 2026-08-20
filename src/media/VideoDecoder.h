#pragma once

#include "FrameRingBuffer.h"
#include <string>
#include <thread>
#include <atomic>
#include <memory>

namespace nf {

enum class VideoLoopMode {
    Loop = 0,
    Once = 1,
    PingPong = 2
};

struct VideoStreamInfo {
    uint32_t width = 1920;
    uint32_t height = 1080;
    double durationSeconds = 10.0;
    double fps = 60.0;
    uint64_t totalFrames = 600;
    bool isLoaded = false;
    std::string filePath;
};

class VideoDecoder {
public:
    VideoDecoder();
    ~VideoDecoder();

    bool OpenFile(const std::string& filePath);
    void Close();

    void Play();
    void Pause();
    void SetSpeed(float speed);
    void SetLoopMode(VideoLoopMode mode);
    void Seek(double timeSeconds);
    void SeekFrame(uint64_t frameIndex);

    bool GetFrame(double currentTime, VideoFrame& outFrame);
    const VideoStreamInfo& GetInfo() const { return m_info; }
    bool IsPlaying() const { return m_isPlaying.load(); }
    double GetCurrentTime() const { return m_currentTime.load(); }

private:
    void StartDecodeThread();
    void StopDecodeThread();
    void DecodeLoop();
    VideoFrame GenerateProceduralFrame(double timestamp, uint64_t frameIndex);

    VideoStreamInfo m_info;
    FrameRingBuffer m_ringBuffer;

    std::atomic<bool> m_isPlaying{true};
    std::atomic<bool> m_shouldStop{false};
    std::atomic<float> m_speed{1.0f};
    std::atomic<int> m_loopMode{static_cast<int>(VideoLoopMode::Loop)};
    std::atomic<double> m_currentTime{0.0};
    std::atomic<bool> m_isPingPongReverse{false};

    std::thread m_decodeThread;
};

} // namespace nf
