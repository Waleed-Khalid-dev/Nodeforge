#include "NoiseChanOp.h"
#include <cmath>
#include <sstream>
#include <algorithm>

namespace nf {

NoiseChanOp::NoiseChanOp(NodeId id, const std::string& name)
    : ChanOp(id, name, "NoiseChanOp") {
    m_outPin = AddOutputPin("output", PinType::Chan);

    SetParam("seed", static_cast<int32_t>(0));
    SetParam("period", 1.0f);
    SetParam("roughness", 0.5f);
    SetParam("octaves", static_cast<int32_t>(3));
    SetParam("amplitude", 1.0f);
    SetParam("offset", 0.0f);
    SetParam("channel_names", std::string("noise1"));
    SetParam("time_sliced", true);
    SetParam("sample_count", static_cast<int32_t>(1));
    SetParam("sample_rate", 60.0f);
}

// 1D pseudo-random hash
static inline float Hash1D(int32_t n, int32_t seed) {
    n += seed * 1337;
    n = (n << 13) ^ n;
    int32_t nn = (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff;
    return 1.0f - (static_cast<float>(nn) / 1073741824.0f); // -1.0 to 1.0
}

float NoiseChanOp::GradientNoise1D(float x, int32_t seed) {
    int32_t x0 = static_cast<int32_t>(std::floor(x));
    int32_t x1 = x0 + 1;
    float dx = x - static_cast<float>(x0);

    // Quintic smooth interpolation curve (6t^5 - 15t^4 + 10t^3)
    float u = dx * dx * dx * (dx * (dx * 6.0f - 15.0f) + 10.0f);

    float g0 = Hash1D(x0, seed);
    float g1 = Hash1D(x1, seed);

    float n0 = g0 * dx;
    float n1 = g1 * (dx - 1.0f);

    return (n0 * (1.0f - u) + n1 * u) * 2.0f;
}

float NoiseChanOp::FBM1D(float x, int32_t octaves, float roughness, int32_t seed) {
    float total = 0.0f;
    float freq = 1.0f;
    float amp = 1.0f;
    float maxAmp = 0.0f;

    for (int32_t i = 0; i < octaves; ++i) {
        total += GradientNoise1D(x * freq, seed + i * 31) * amp;
        maxAmp += amp;
        freq *= 2.0f;
        amp *= roughness;
    }

    return (maxAmp > 0.0f) ? (total / maxAmp) : 0.0f;
}

bool NoiseChanOp::Cook(const CookContext& context) {
    int32_t seed = GetParam("seed").Is<int32_t>() ? GetParam("seed").Get<int32_t>() : 0;
    float period = GetParam("period").Is<float>() ? GetParam("period").Get<float>() : 1.0f;
    period = std::max(period, 0.0001f);
    float roughness = GetParam("roughness").Is<float>() ? GetParam("roughness").Get<float>() : 0.5f;
    int32_t octaves = GetParam("octaves").Is<int32_t>() ? std::clamp(GetParam("octaves").Get<int32_t>(), 1, 8) : 3;
    float amp = GetParam("amplitude").Is<float>() ? GetParam("amplitude").Get<float>() : 1.0f;
    float offset = GetParam("offset").Is<float>() ? GetParam("offset").Get<float>() : 0.0f;

    std::string chanNamesStr = GetParam("channel_names").Is<std::string>() ? GetParam("channel_names").Get<std::string>() : "noise1";
    bool timeSliced = GetParam("time_sliced").Is<bool>() ? GetParam("time_sliced").Get<bool>() : true;
    float sampleRate = GetParam("sample_rate").Is<float>() ? GetParam("sample_rate").Get<float>() : 60.0f;

    std::vector<std::string> names;
    std::stringstream ss(chanNamesStr);
    std::string item;
    while (ss >> item) {
        names.push_back(item);
    }
    if (names.empty()) {
        names.push_back("noise1");
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

        int32_t chanSeed = seed + static_cast<int32_t>(c) * 1013;

        for (size_t s = 0; s < sampleCount; ++s) {
            double curTime = timeSliced ? (context.timeSeconds + static_cast<double>(s) * dt) : (static_cast<double>(s) * dt);
            float x = static_cast<float>(curTime / period);
            float noiseVal = FBM1D(x, octaves, roughness, chanSeed);
            data[s] = offset + amp * noiseVal;
        }
    }

    SetOutputBuffer(buf);
    return true;
}

} // namespace nf
