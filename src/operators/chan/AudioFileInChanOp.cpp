#include "AudioFileInChanOp.h"
#include <fstream>
#include <cmath>
#include <algorithm>
#include <numbers>

namespace nf {

AudioFileInChanOp::AudioFileInChanOp(NodeId id, const std::string& name)
    : ChanOp(id, name, "AudioFileInChanOp") {
    m_outPin = AddOutputPin("output", PinType::Chan);

    SetParam("file_path", std::string(""));
    SetParam("play", true);
    SetParam("loop", true);
    SetParam("volume", 1.0f);
    SetParam("time_sliced", true);
    SetParam("sample_rate", 44100.0f);
}

bool AudioFileInChanOp::LoadAudioFile(const std::string& filepath) {
    if (filepath == m_loadedPath && !m_audioDataL.empty()) {
        return true;
    }

    m_audioDataL.clear();
    m_audioDataR.clear();
    m_loadedPath = filepath;

    if (filepath.empty()) {
        // Synthesize 1 second of 440 Hz test tone + 880 Hz harmonic
        size_t testSamples = 44100;
        m_fileSampleRate = 44100.0f;
        m_audioDataL.resize(testSamples);
        m_audioDataR.resize(testSamples);
        for (size_t i = 0; i < testSamples; ++i) {
            float t = static_cast<float>(i) / 44100.0f;
            m_audioDataL[i] = 0.5f * std::sin(2.0f * static_cast<float>(std::numbers::pi) * 440.0f * t);
            m_audioDataR[i] = 0.5f * std::sin(2.0f * static_cast<float>(std::numbers::pi) * 880.0f * t);
        }
        return true;
    }

    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    char chunkId[4];
    uint32_t chunkSize = 0;
    char format[4];

    file.read(chunkId, 4);
    file.read(reinterpret_cast<char*>(&chunkSize), 4);
    file.read(format, 4);

    if (std::strncmp(chunkId, "RIFF", 4) != 0 || std::strncmp(format, "WAVE", 4) != 0) {
        return false;
    }

    uint16_t numChannels = 0;
    uint32_t sampleRate = 0;
    uint16_t bitsPerSample = 0;
    std::vector<uint8_t> pcmData;

    while (file.read(chunkId, 4)) {
        file.read(reinterpret_cast<char*>(&chunkSize), 4);
        if (std::strncmp(chunkId, "fmt ", 4) == 0) {
            uint16_t audioFormat = 0;
            file.read(reinterpret_cast<char*>(&audioFormat), 2);
            file.read(reinterpret_cast<char*>(&numChannels), 2);
            file.read(reinterpret_cast<char*>(&sampleRate), 4);
            file.seekg(6, std::ios::cur); // byteRate (4) + blockAlign (2)
            file.read(reinterpret_cast<char*>(&bitsPerSample), 2);
            if (chunkSize > 16) {
                file.seekg(chunkSize - 16, std::ios::cur);
            }
        } else if (std::strncmp(chunkId, "data", 4) == 0) {
            pcmData.resize(chunkSize);
            file.read(reinterpret_cast<char*>(pcmData.data()), chunkSize);
            break;
        } else {
            file.seekg(chunkSize, std::ios::cur);
        }
    }

    if (numChannels == 0 || sampleRate == 0 || bitsPerSample == 0 || pcmData.empty()) {
        return false;
    }

    m_fileSampleRate = static_cast<float>(sampleRate);
    size_t bytesPerSample = bitsPerSample / 8;
    size_t totalSamples = pcmData.size() / (numChannels * bytesPerSample);

    m_audioDataL.resize(totalSamples);
    m_audioDataR.resize(totalSamples);

    for (size_t i = 0; i < totalSamples; ++i) {
        for (size_t c = 0; c < numChannels; ++c) {
            size_t offset = (i * numChannels + c) * bytesPerSample;
            float val = 0.0f;
            if (bitsPerSample == 16) {
                int16_t raw = *reinterpret_cast<const int16_t*>(&pcmData[offset]);
                val = static_cast<float>(raw) / 32768.0f;
            } else if (bitsPerSample == 8) {
                uint8_t raw = pcmData[offset];
                val = (static_cast<float>(raw) - 128.0f) / 128.0f;
            } else if (bitsPerSample == 32) {
                val = *reinterpret_cast<const float*>(&pcmData[offset]);
            }

            if (c == 0) m_audioDataL[i] = val;
            else if (c == 1) m_audioDataR[i] = val;
        }
        if (numChannels == 1) {
            m_audioDataR[i] = m_audioDataL[i];
        }
    }

    return true;
}

bool AudioFileInChanOp::Cook(const CookContext& context) {
    std::string path = GetParam("file_path").Is<std::string>() ? GetParam("file_path").Get<std::string>() : "";
    bool play = GetParam("play").Is<bool>() ? GetParam("play").Get<bool>() : true;
    bool loop = GetParam("loop").Is<bool>() ? GetParam("loop").Get<bool>() : true;
    float volume = GetParam("volume").Is<float>() ? GetParam("volume").Get<float>() : 1.0f;
    bool timeSliced = GetParam("time_sliced").Is<bool>() ? GetParam("time_sliced").Get<bool>() : true;
    float targetSampleRate = GetParam("sample_rate").Is<float>() ? GetParam("sample_rate").Get<float>() : 44100.0f;

    LoadAudioFile(path);

    if (m_audioDataL.empty()) {
        SetOutputBuffer(ChannelBuffer({ "chan1", "chan2" }, 1, targetSampleRate));
        return true;
    }

    std::vector<std::string> names = { "chan1", "chan2" };
    size_t sampleCount = 1;

    if (timeSliced) {
        sampleCount = static_cast<size_t>(targetSampleRate * static_cast<float>(context.deltaTimeSeconds));
        if (sampleCount == 0) sampleCount = 1;
    } else {
        sampleCount = m_audioDataL.size();
    }

    ChannelBuffer buf(names, sampleCount, targetSampleRate, context.timeSlice.startSample);
    float* outL = buf.GetChannelData(0);
    float* outR = buf.GetChannelData(1);

    size_t totalAudioSamples = m_audioDataL.size();

    for (size_t s = 0; s < sampleCount; ++s) {
        size_t idx = static_cast<size_t>(m_playCursor);
        if (idx >= totalAudioSamples) {
            if (loop && totalAudioSamples > 0) {
                m_playCursor = 0.0;
                idx = 0;
            } else {
                idx = totalAudioSamples > 0 ? totalAudioSamples - 1 : 0;
            }
        }

        if (outL) outL[s] = m_audioDataL[idx] * volume;
        if (outR) outR[s] = m_audioDataR[idx] * volume;

        if (play && timeSliced) {
            m_playCursor += 1.0;
        }
    }

    SetOutputBuffer(buf);
    return true;
}

} // namespace nf
