#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <span>
#include <stdexcept>

namespace nf {

/// High-performance SIMD-aligned contiguous multi-channel numeric buffer.
/// Stores planar channel float data in a cache-coherent flat memory buffer.
class ChannelBuffer {
public:
    ChannelBuffer() = default;

    /// Allocate buffer with specified dimensions
    ChannelBuffer(const std::vector<std::string>& channelNames, size_t sampleCount, float rate = 60.0f, int64_t start = 0);

    // Dimension queries
    size_t GetChannelCount() const { return m_names.size(); }
    size_t GetSampleCount() const { return m_sampleCount; }
    size_t GetTotalElements() const { return m_data.size(); }
    float GetSampleRate() const { return m_sampleRate; }
    int64_t GetStartSample() const { return m_startSample; }
    bool IsEmpty() const { return m_names.empty() || m_sampleCount == 0 || m_data.empty(); }

    void SetSampleRate(float rate) { m_sampleRate = rate; }
    void SetStartSample(int64_t start) { m_startSample = start; }

    const std::vector<std::string>& GetChannelNames() const { return m_names; }
    void SetChannelNames(const std::vector<std::string>& names);

    int FindChannelIndex(const std::string& name) const;
    bool HasChannel(const std::string& name) const { return FindChannelIndex(name) >= 0; }

    // Channel creation & resizing
    void Resize(size_t channelCount, size_t sampleCount);
    void AddChannel(const std::string& name, const std::vector<float>& samples);
    void AddChannel(const std::string& name, const float* samples, size_t sampleCount);
    void Clear();

    // Direct memory access
    float* GetChannelData(size_t channelIdx);
    const float* GetChannelData(size_t channelIdx) const;

    float* GetChannelData(const std::string& name);
    const float* GetChannelData(const std::string& name) const;

    std::span<float> GetChannelSpan(size_t channelIdx);
    std::span<const float> GetChannelSpan(size_t channelIdx) const;

    const std::vector<float>& GetRawData() const { return m_data; }
    std::vector<float>& GetRawData() { return m_data; }

    // Sample access
    float GetSample(size_t channelIdx, size_t sampleIdx) const;
    float GetSample(const std::string& name, size_t sampleIdx) const;
    void SetSample(size_t channelIdx, size_t sampleIdx, float value);
    void SetSample(const std::string& name, size_t sampleIdx, float value);

    // Mathematical operations (SIMD-accelerated / vectorized loops)
    void Add(float scalar);
    void Add(const ChannelBuffer& other);
    void Multiply(float scalar);
    void Multiply(const ChannelBuffer& other);
    void Remap(float inMin, float inMax, float outMin, float outMax);
    void Clamp(float minVal, float maxVal);
    void Abs();
    void Sin();
    void Cos();

    // Slicing & Resampling
    ChannelBuffer Slice(size_t startSample, size_t count) const;
    ChannelBuffer Resample(size_t newSampleCount) const;

private:
    std::vector<std::string> m_names;
    std::vector<float> m_data; // Flat contiguous storage: channel 0, channel 1, ...
    size_t m_sampleCount = 0;
    float m_sampleRate = 60.0f;
    int64_t m_startSample = 0;
};

} // namespace nf
