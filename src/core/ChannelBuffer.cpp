#include "ChannelBuffer.h"
#include <cmath>
#include <cstring>
#include <algorithm>

namespace nf {

ChannelBuffer::ChannelBuffer(const std::vector<std::string>& channelNames, size_t sampleCount, float rate, int64_t start)
    : m_names(channelNames), m_sampleCount(sampleCount), m_sampleRate(rate), m_startSample(start) {
    m_data.resize(m_names.size() * m_sampleCount, 0.0f);
}

void ChannelBuffer::SetChannelNames(const std::vector<std::string>& newNames) {
    m_names = newNames;
    m_data.resize(m_names.size() * m_sampleCount, 0.0f);
}

int ChannelBuffer::FindChannelIndex(const std::string& name) const {
    for (size_t i = 0; i < m_names.size(); ++i) {
        if (m_names[i] == name) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

void ChannelBuffer::Resize(size_t channelCount, size_t sampleCount) {
    if (channelCount != m_names.size()) {
        m_names.resize(channelCount);
        for (size_t i = 0; i < channelCount; ++i) {
            if (m_names[i].empty()) {
                m_names[i] = "chan" + std::to_string(i + 1);
            }
        }
    }
    m_sampleCount = sampleCount;
    m_data.assign(channelCount * sampleCount, 0.0f);
}

void ChannelBuffer::AddChannel(const std::string& name, const std::vector<float>& samples) {
    AddChannel(name, samples.data(), samples.size());
}

void ChannelBuffer::AddChannel(const std::string& name, const float* samples, size_t count) {
    if (m_names.empty()) {
        m_sampleCount = count;
    } else if (m_sampleCount != count) {
        if (m_sampleCount == 0) {
            m_sampleCount = count;
        }
    }

    m_names.push_back(name);
    size_t prevSize = m_data.size();
    m_data.resize(prevSize + m_sampleCount, 0.0f);

    size_t copyCount = std::min(count, m_sampleCount);
    if (samples && copyCount > 0) {
        std::memcpy(&m_data[prevSize], samples, copyCount * sizeof(float));
    }
}

void ChannelBuffer::Clear() {
    m_names.clear();
    m_data.clear();
    m_sampleCount = 0;
}

float* ChannelBuffer::GetChannelData(size_t channelIdx) {
    if (channelIdx >= m_names.size() || m_sampleCount == 0) return nullptr;
    return &m_data[channelIdx * m_sampleCount];
}

const float* ChannelBuffer::GetChannelData(size_t channelIdx) const {
    if (channelIdx >= m_names.size() || m_sampleCount == 0) return nullptr;
    return &m_data[channelIdx * m_sampleCount];
}

float* ChannelBuffer::GetChannelData(const std::string& name) {
    int idx = FindChannelIndex(name);
    return idx >= 0 ? GetChannelData(static_cast<size_t>(idx)) : nullptr;
}

const float* ChannelBuffer::GetChannelData(const std::string& name) const {
    int idx = FindChannelIndex(name);
    return idx >= 0 ? GetChannelData(static_cast<size_t>(idx)) : nullptr;
}

std::span<float> ChannelBuffer::GetChannelSpan(size_t channelIdx) {
    float* ptr = GetChannelData(channelIdx);
    if (!ptr) return {};
    return std::span<float>(ptr, m_sampleCount);
}

std::span<const float> ChannelBuffer::GetChannelSpan(size_t channelIdx) const {
    const float* ptr = GetChannelData(channelIdx);
    if (!ptr) return {};
    return std::span<const float>(ptr, m_sampleCount);
}

float ChannelBuffer::GetSample(size_t channelIdx, size_t sampleIdx) const {
    if (channelIdx >= m_names.size() || sampleIdx >= m_sampleCount) return 0.0f;
    return m_data[channelIdx * m_sampleCount + sampleIdx];
}

float ChannelBuffer::GetSample(const std::string& name, size_t sampleIdx) const {
    int idx = FindChannelIndex(name);
    return idx >= 0 ? GetSample(static_cast<size_t>(idx), sampleIdx) : 0.0f;
}

void ChannelBuffer::SetSample(size_t channelIdx, size_t sampleIdx, float value) {
    if (channelIdx < m_names.size() && sampleIdx < m_sampleCount) {
        m_data[channelIdx * m_sampleCount + sampleIdx] = value;
    }
}

void ChannelBuffer::SetSample(const std::string& name, size_t sampleIdx, float value) {
    int idx = FindChannelIndex(name);
    if (idx >= 0) {
        SetSample(static_cast<size_t>(idx), sampleIdx, value);
    }
}

void ChannelBuffer::Add(float scalar) {
    const size_t n = m_data.size();
    float* d = m_data.data();
    for (size_t i = 0; i < n; ++i) {
        d[i] += scalar;
    }
}

void ChannelBuffer::Add(const ChannelBuffer& other) {
    size_t chans = std::min(m_names.size(), other.m_names.size());
    size_t samples = std::min(m_sampleCount, other.m_sampleCount);
    for (size_t c = 0; c < chans; ++c) {
        float* dst = GetChannelData(c);
        const float* src = other.GetChannelData(c);
        for (size_t s = 0; s < samples; ++s) {
            dst[s] += src[s];
        }
    }
}

void ChannelBuffer::Multiply(float scalar) {
    const size_t n = m_data.size();
    float* d = m_data.data();
    for (size_t i = 0; i < n; ++i) {
        d[i] *= scalar;
    }
}

void ChannelBuffer::Multiply(const ChannelBuffer& other) {
    size_t chans = std::min(m_names.size(), other.m_names.size());
    size_t samples = std::min(m_sampleCount, other.m_sampleCount);
    for (size_t c = 0; c < chans; ++c) {
        float* dst = GetChannelData(c);
        const float* src = other.GetChannelData(c);
        for (size_t s = 0; s < samples; ++s) {
            dst[s] *= src[s];
        }
    }
}

void ChannelBuffer::Remap(float inMin, float inMax, float outMin, float outMax) {
    float denom = (std::abs(inMax - inMin) > 1e-7f) ? (inMax - inMin) : 1.0f;
    float scale = (outMax - outMin) / denom;
    const size_t n = m_data.size();
    float* d = m_data.data();
    for (size_t i = 0; i < n; ++i) {
        d[i] = outMin + (d[i] - inMin) * scale;
    }
}

void ChannelBuffer::Clamp(float minVal, float maxVal) {
    const size_t n = m_data.size();
    float* d = m_data.data();
    for (size_t i = 0; i < n; ++i) {
        d[i] = std::clamp(d[i], minVal, maxVal);
    }
}

void ChannelBuffer::Abs() {
    const size_t n = m_data.size();
    float* d = m_data.data();
    for (size_t i = 0; i < n; ++i) {
        d[i] = std::abs(d[i]);
    }
}

void ChannelBuffer::Sin() {
    const size_t n = m_data.size();
    float* d = m_data.data();
    for (size_t i = 0; i < n; ++i) {
        d[i] = std::sin(d[i]);
    }
}

void ChannelBuffer::Cos() {
    const size_t n = m_data.size();
    float* d = m_data.data();
    for (size_t i = 0; i < n; ++i) {
        d[i] = std::cos(d[i]);
    }
}

ChannelBuffer ChannelBuffer::Slice(size_t startSample, size_t count) const {
    ChannelBuffer result;
    result.m_names = m_names;
    result.m_sampleRate = m_sampleRate;
    result.m_startSample = m_startSample + static_cast<int64_t>(startSample);
    result.m_sampleCount = count;
    result.m_data.resize(m_names.size() * count, 0.0f);

    for (size_t c = 0; c < m_names.size(); ++c) {
        const float* src = GetChannelData(c);
        float* dst = result.GetChannelData(c);
        if (startSample < m_sampleCount) {
            size_t available = std::min(count, m_sampleCount - startSample);
            std::memcpy(dst, &src[startSample], available * sizeof(float));
        }
    }
    return result;
}

ChannelBuffer ChannelBuffer::Resample(size_t newSampleCount) const {
    if (newSampleCount == 0 || m_sampleCount == 0 || m_names.empty()) {
        return ChannelBuffer(m_names, newSampleCount, m_sampleRate, m_startSample);
    }

    ChannelBuffer result(m_names, newSampleCount, m_sampleRate, m_startSample);
    float step = (m_sampleCount > 1) ? static_cast<float>(m_sampleCount - 1) / static_cast<float>(newSampleCount - 1) : 0.0f;

    for (size_t c = 0; c < m_names.size(); ++c) {
        const float* src = GetChannelData(c);
        float* dst = result.GetChannelData(c);
        for (size_t i = 0; i < newSampleCount; ++i) {
            float srcPos = static_cast<float>(i) * step;
            size_t idx0 = static_cast<size_t>(srcPos);
            size_t idx1 = std::min(idx0 + 1, m_sampleCount - 1);
            float frac = srcPos - static_cast<float>(idx0);
            dst[i] = src[idx0] * (1.0f - frac) + src[idx1] * frac;
        }
    }
    return result;
}

} // namespace nf
