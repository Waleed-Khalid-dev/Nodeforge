#include "AudioSpatializerChanOp.h"
#include <cmath>
#include <algorithm>

namespace nf {

AudioSpatializerChanOp::AudioSpatializerChanOp(NodeId id, const std::string& name)
    : Node(id, name, "AudioSpatializerChanOp") {
    m_inAudioPin = AddInputPin("in_audio", PinType::Chan);
    m_inPosPin = AddInputPin("in_pos", PinType::Chan);
    m_outAudioPin = AddOutputPin("out_audio", PinType::Chan);

    SetParam("spatial_mode", static_cast<int32_t>(1)); // 0: Ambisonics B-Format, 1: VBAP Multi-Speaker
    SetParam("speaker_layout", static_cast<int32_t>(2)); // 0: Stereo, 1: Quad, 2: 5.1, 3: 7.1.4, 4: Octaphonic, 5: 16-Ch
    SetParam("emitter_pos", glm::vec3(0.0f, 0.0f, 0.0f));
    SetParam("listener_pos", glm::vec3(0.0f, 0.0f, 0.0f));
    SetParam("listener_rot", glm::vec3(0.0f, 0.0f, 0.0f));
    SetParam("attenuation_rolloff", 1.0f);
    SetParam("max_distance", 100.0f);
    SetParam("doppler_factor", 1.0f);

    m_outBuffer.Resize(2, 1);
    m_outBuffer.SetChannelNames({"ch_L", "ch_R"});
}

glm::vec3 AudioSpatializerChanOp::GetEmitterPosition() const {
    return GetParam("emitter_pos").Is<glm::vec3>() ? GetParam("emitter_pos").Get<glm::vec3>() : glm::vec3(0.0f);
}

glm::vec3 AudioSpatializerChanOp::GetListenerPosition() const {
    return GetParam("listener_pos").Is<glm::vec3>() ? GetParam("listener_pos").Get<glm::vec3>() : glm::vec3(0.0f);
}

float AudioSpatializerChanOp::GetDistanceAttenuation() const {
    glm::vec3 dVec = GetEmitterPosition() - GetListenerPosition();
    float dist = glm::length(dVec);
    float maxDist = GetParam("max_distance").Is<float>() ? GetParam("max_distance").Get<float>() : 100.0f;
    if (dist > maxDist) return 0.0f;

    float rolloff = GetParam("attenuation_rolloff").Is<float>() ? GetParam("attenuation_rolloff").Get<float>() : 1.0f;
    return 1.0f / (1.0f + rolloff * dist * 0.1f);
}

bool AudioSpatializerChanOp::Cook(const CookContext& /*context*/) {
    // Read dynamic emitter position from in_pos if provided
    glm::vec3 emitterPos = GetEmitterPosition();
    if (m_inPosPin && m_inPosPin->GetValue().Is<ChannelBuffer>()) {
        const auto& posBuf = m_inPosPin->GetValue().Get<ChannelBuffer>();
        if (posBuf.GetChannelCount() >= 3 && posBuf.GetSampleCount() > 0) {
            emitterPos.x = posBuf.GetChannelData(0)[0];
            emitterPos.y = posBuf.GetChannelData(1)[0];
            emitterPos.z = posBuf.GetChannelData(2)[0];
        }
    }

    glm::vec3 listenerPos = GetListenerPosition();
    glm::vec3 dVec = emitterPos - listenerPos;
    float dist = glm::length(dVec);
    float attenuation = GetDistanceAttenuation();

    // Spherical angles
    float azimuth = (dist > 0.001f) ? std::atan2(dVec.x, -dVec.z) : 0.0f;
    float elevation = (dist > 0.001f) ? std::asin(std::clamp(dVec.y / dist, -1.0f, 1.0f)) : 0.0f;

    // Get input audio samples or generate synthetic carrier
    size_t sampleCount = 1;
    std::vector<float> inSamples = {1.0f};

    if (m_inAudioPin && m_inAudioPin->GetValue().Is<ChannelBuffer>()) {
        const auto& inBuf = m_inAudioPin->GetValue().Get<ChannelBuffer>();
        if (inBuf.GetChannelCount() > 0 && inBuf.GetSampleCount() > 0) {
            sampleCount = inBuf.GetSampleCount();
            inSamples.resize(sampleCount);
            const float* src = inBuf.GetChannelData(0);
            std::copy(src, src + sampleCount, inSamples.begin());
        }
    }

    int32_t spatialMode = GetParam("spatial_mode").Is<int32_t>() ? GetParam("spatial_mode").Get<int32_t>() : 1;

    if (spatialMode == 0) {
        // Ambisonics B-Format (W, X, Y, Z)
        m_outBuffer.Resize(4, sampleCount);
        m_outBuffer.SetChannelNames({"W", "X", "Y", "Z"});

        float wGain = 0.70710678f * attenuation; // 1 / sqrt(2)
        float xGain = std::cos(azimuth) * std::cos(elevation) * attenuation;
        float yGain = std::sin(azimuth) * std::cos(elevation) * attenuation;
        float zGain = std::sin(elevation) * attenuation;

        float* wData = m_outBuffer.GetChannelData(0);
        float* xData = m_outBuffer.GetChannelData(1);
        float* yData = m_outBuffer.GetChannelData(2);
        float* zData = m_outBuffer.GetChannelData(3);

        for (size_t s = 0; s < sampleCount; ++s) {
            float samp = inSamples[s];
            wData[s] = samp * wGain;
            xData[s] = samp * xGain;
            yData[s] = samp * yGain;
            zData[s] = samp * zGain;
        }
    } else {
        // VBAP Multi-Speaker Layout Mode
        int32_t layout = GetParam("speaker_layout").Is<int32_t>() ? GetParam("speaker_layout").Get<int32_t>() : 2;

        if (layout == 0) {
            // Stereo (Left, Right)
            m_outBuffer.Resize(2, sampleCount);
            m_outBuffer.SetChannelNames({"ch_L", "ch_R"});

            float pan = std::clamp((azimuth / (3.14159265f * 0.5f) + 1.0f) * 0.5f, 0.0f, 1.0f);
            float gL = std::cos(pan * 1.5707963f) * attenuation;
            float gR = std::sin(pan * 1.5707963f) * attenuation;

            float* lData = m_outBuffer.GetChannelData(0);
            float* rData = m_outBuffer.GetChannelData(1);

            for (size_t s = 0; s < sampleCount; ++s) {
                lData[s] = inSamples[s] * gL;
                rData[s] = inSamples[s] * gR;
            }
        } else if (layout == 2) {
            // 5.1 Surround (L, R, C, LFE, Ls, Rs)
            m_outBuffer.Resize(6, sampleCount);
            m_outBuffer.SetChannelNames({"ch_L", "ch_R", "ch_C", "ch_LFE", "ch_Ls", "ch_Rs"});

            float normAz = azimuth;
            float gL = std::max(0.0f, std::cos(normAz - (-0.5235f))) * attenuation; // -30 deg
            float gR = std::max(0.0f, std::cos(normAz - (0.5235f))) * attenuation;  // +30 deg
            float gC = std::max(0.0f, std::cos(normAz - 0.0f)) * attenuation;       // 0 deg
            float gLFE = 0.5f * attenuation;
            float gLs = std::max(0.0f, std::cos(normAz - (-1.9198f))) * attenuation;// -110 deg
            float gRs = std::max(0.0f, std::cos(normAz - (1.9198f))) * attenuation; // +110 deg

            for (size_t ch = 0; ch < 6; ++ch) {
                float gain = (ch == 0) ? gL : (ch == 1) ? gR : (ch == 2) ? gC : (ch == 3) ? gLFE : (ch == 4) ? gLs : gRs;
                float* d = m_outBuffer.GetChannelData(ch);
                for (size_t s = 0; s < sampleCount; ++s) {
                    d[s] = inSamples[s] * gain;
                }
            }
        } else if (layout == 3) {
            // 7.1.4 Dolby Atmos Dome (12 channels)
            m_outBuffer.Resize(12, sampleCount);
            m_outBuffer.SetChannelNames({
                "ch_L", "ch_R", "ch_C", "ch_LFE",
                "ch_Ls", "ch_Rs", "ch_Rls", "ch_Rrs",
                "ch_Tfl", "ch_Tfr", "ch_Trl", "ch_Trr"
            });

            float heightGain = std::max(0.0f, std::sin(elevation));
            float floorGain = std::max(0.0f, std::cos(elevation));

            for (size_t ch = 0; ch < 12; ++ch) {
                float angle = static_cast<float>(ch % 8) * (6.2831853f / 8.0f) - 3.14159265f;
                float angleGain = std::max(0.0f, std::cos(azimuth - angle));
                float vertical = (ch >= 8) ? heightGain : floorGain;
                float totalGain = angleGain * vertical * attenuation;

                float* d = m_outBuffer.GetChannelData(ch);
                for (size_t s = 0; s < sampleCount; ++s) {
                    d[s] = inSamples[s] * totalGain;
                }
            }
        } else {
            // Quad 4.0 / General
            m_outBuffer.Resize(4, sampleCount);
            m_outBuffer.SetChannelNames({"ch_FL", "ch_FR", "ch_RL", "ch_RR"});
            for (size_t ch = 0; ch < 4; ++ch) {
                float angle = static_cast<float>(ch) * 1.5707963f - 0.785398f;
                float g = std::max(0.0f, std::cos(azimuth - angle)) * attenuation;
                float* d = m_outBuffer.GetChannelData(ch);
                for (size_t s = 0; s < sampleCount; ++s) {
                    d[s] = inSamples[s] * g;
                }
            }
        }
    }

    m_outAudioPin->SetValue(PinValue(m_outBuffer));
    return true;
}

} // namespace nf
