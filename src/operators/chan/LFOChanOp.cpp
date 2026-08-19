#include "LFOChanOp.h"
#include <cmath>
#include <sstream>
#include <numbers>

namespace nf {

LFOChanOp::LFOChanOp(NodeId id, const std::string& name)
    : ChanOp(id, name, "LFOChanOp") {
    AddInputPin("reset", PinType::Chan);
    m_outPin = AddOutputPin("output", PinType::Chan);

    SetParam("type", static_cast<int32_t>(0)); // 0: Sine, 1: Tri, 2: Saw, 3: Square, 4: Pulse, 5: Perlin
    SetParam("frequency", 1.0f);
    SetParam("amplitude", 1.0f);
    SetParam("offset", 0.0f);
    SetParam("phase", 0.0f);
    SetParam("pulse_width", 0.5f);
    SetParam("channel_names", std::string("chan1"));
    SetParam("time_sliced", true);
    SetParam("sample_count", static_cast<int32_t>(1));
    SetParam("sample_rate", 60.0f);
}

static float EvaluateWaveform(int32_t type, float phase, float pulseWidth) {
    float p = phase - std::floor(phase); // [0, 1)

    switch (type) {
        case 0: // Sine
            return std::sin(p * 2.0f * static_cast<float>(std::numbers::pi));
        case 1: { // Triangle
            float tp = p + 0.25f;
            tp = tp - std::floor(tp);
            return 1.0f - 4.0f * std::abs(tp - 0.5f);
        }
        case 2: // Sawtooth
            return 2.0f * p - 1.0f;
        case 3: // Square (-1 to +1)
            return (p < pulseWidth) ? 1.0f : -1.0f;
        case 4: // Pulse (0 to 1)
            return (p < pulseWidth) ? 1.0f : 0.0f;
        case 5: { // Smooth cubic Hermite wave
            float s = std::sin(p * 2.0f * static_cast<float>(std::numbers::pi));
            return s * s * (3.0f - 2.0f * s) * (s < 0 ? -1.0f : 1.0f);
        }
        default:
            return std::sin(p * 2.0f * static_cast<float>(std::numbers::pi));
    }
}

bool LFOChanOp::Cook(const CookContext& context) {
    // Check reset input
    const ChannelBuffer* resetBuf = GetInputBuffer(0);
    if (resetBuf && !resetBuf->IsEmpty() && resetBuf->GetSample(0, 0) > 0.5f) {
        m_phaseAccum = 0.0;
    }

    int32_t type = GetParam("type").Is<int32_t>() ? GetParam("type").Get<int32_t>() : 0;
    float freq = GetParam("frequency").Is<float>() ? GetParam("frequency").Get<float>() : 1.0f;
    float amp = GetParam("amplitude").Is<float>() ? GetParam("amplitude").Get<float>() : 1.0f;
    float offset = GetParam("offset").Is<float>() ? GetParam("offset").Get<float>() : 0.0f;
    float phaseShift = GetParam("phase").Is<float>() ? GetParam("phase").Get<float>() : 0.0f;
    float pulseWidth = GetParam("pulse_width").Is<float>() ? GetParam("pulse_width").Get<float>() : 0.5f;
    pulseWidth = std::clamp(pulseWidth, 0.001f, 0.999f);

    std::string chanNamesStr = GetParam("channel_names").Is<std::string>() ? GetParam("channel_names").Get<std::string>() : "chan1";
    bool timeSliced = GetParam("time_sliced").Is<bool>() ? GetParam("time_sliced").Get<bool>() : true;
    float sampleRate = GetParam("sample_rate").Is<float>() ? GetParam("sample_rate").Get<float>() : 60.0f;

    std::vector<std::string> names;
    std::stringstream ss(chanNamesStr);
    std::string item;
    while (ss >> item) {
        names.push_back(item);
    }
    if (names.empty()) {
        names.push_back("chan1");
    }

    size_t sampleCount = 1;
    if (timeSliced) {
        sampleCount = context.timeSlice.sampleCount > 0 ? context.timeSlice.sampleCount : 1;
    } else {
        if (GetParam("sample_count").Is<int32_t>()) {
            sampleCount = std::max(1, GetParam("sample_count").Get<int32_t>());
        }
    }

    ChannelBuffer buf(names, sampleCount, sampleRate, context.timeSlice.startSample);

    double dt = (sampleRate > 1e-6f) ? (1.0 / static_cast<double>(sampleRate)) : (1.0 / 60.0);

    for (size_t c = 0; c < names.size(); ++c) {
        float* data = buf.GetChannelData(c);
        if (!data) continue;

        // Channel phase offset based on channel index
        float channelPhase = phaseShift + static_cast<float>(c) * 0.25f;

        for (size_t s = 0; s < sampleCount; ++s) {
            double curTime = timeSliced ? (context.timeSeconds + static_cast<double>(s) * dt) : (static_cast<double>(s) * dt);
            float currentPhase = static_cast<float>(curTime * freq) + channelPhase;
            float rawVal = EvaluateWaveform(type, currentPhase, pulseWidth);
            data[s] = offset + amp * rawVal;
        }
    }

    SetOutputBuffer(buf);
    return true;
}

} // namespace nf
