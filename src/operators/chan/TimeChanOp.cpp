#include "TimeChanOp.h"

namespace nf {

TimeChanOp::TimeChanOp(NodeId id, const std::string& name)
    : ChanOp(id, name, "TimeChanOp") {
    m_outPin = AddOutputPin("output", PinType::Chan);

    SetParam("output_seconds", true);
    SetParam("output_frames", true);
    SetParam("output_fraction", true);
    SetParam("output_rate", false);
    SetParam("sample_rate", 60.0f);
}

bool TimeChanOp::Cook(const CookContext& context) {
    bool outSec = GetParam("output_seconds").Is<bool>() ? GetParam("output_seconds").Get<bool>() : true;
    bool outFrames = GetParam("output_frames").Is<bool>() ? GetParam("output_frames").Get<bool>() : true;
    bool outFrac = GetParam("output_fraction").Is<bool>() ? GetParam("output_fraction").Get<bool>() : true;
    bool outRate = GetParam("output_rate").Is<bool>() ? GetParam("output_rate").Get<bool>() : false;

    float sampleRate = 60.0f;
    if (GetParam("sample_rate").Is<float>()) {
        sampleRate = GetParam("sample_rate").Get<float>();
    }

    std::vector<std::string> names;
    if (outSec) names.push_back("seconds");
    if (outFrames) names.push_back("frame");
    if (outFrac) names.push_back("fraction");
    if (outRate) names.push_back("rate");

    if (names.empty()) {
        names.push_back("seconds");
    }

    size_t sampleCount = 1;
    if (context.timeSlice.isTimeSliced && context.timeSlice.sampleCount > 0) {
        sampleCount = context.timeSlice.sampleCount;
    }

    ChannelBuffer buf(names, sampleCount, sampleRate, context.timeSlice.startSample);

    float baseSeconds = static_cast<float>(context.timeSeconds);
    float baseFrame = static_cast<float>(context.frameIndex);
    float rateVal = (context.deltaTimeSeconds > 1e-6) ? static_cast<float>(1.0 / context.deltaTimeSeconds) : sampleRate;
    float dt = static_cast<float>(context.deltaTimeSeconds) / static_cast<float>(sampleCount);

    for (size_t i = 0; i < names.size(); ++i) {
        float* data = buf.GetChannelData(i);
        if (!data) continue;

        if (names[i] == "seconds") {
            for (size_t s = 0; s < sampleCount; ++s) {
                data[s] = baseSeconds + static_cast<float>(s) * dt;
            }
        } else if (names[i] == "frame") {
            for (size_t s = 0; s < sampleCount; ++s) {
                data[s] = baseFrame + static_cast<float>(s);
            }
        } else if (names[i] == "fraction") {
            for (size_t s = 0; s < sampleCount; ++s) {
                float sec = baseSeconds + static_cast<float>(s) * dt;
                data[s] = std::fmod(sec, 1.0f);
                if (data[s] < 0.0f) data[s] += 1.0f;
            }
        } else if (names[i] == "rate") {
            std::fill_n(data, sampleCount, rateVal);
        }
    }

    SetOutputBuffer(buf);
    return true;
}

} // namespace nf
