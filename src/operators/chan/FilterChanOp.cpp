#include "FilterChanOp.h"
#include <cmath>
#include <numbers>
#include <algorithm>

namespace nf {

FilterChanOp::FilterChanOp(NodeId id, const std::string& name)
    : ChanOp(id, name, "FilterChanOp") {
    AddInputPin("input", PinType::Chan);
    AddInputPin("reset", PinType::Chan);
    m_outPin = AddOutputPin("output", PinType::Chan);

    SetParam("type", static_cast<int32_t>(0)); // 0: Lag, 1: Spring, 2: Box (Moving Average)
    SetParam("lag_up", 0.2f);
    SetParam("lag_down", 0.2f);
    SetParam("frequency", 2.0f);
    SetParam("damping", 0.707f);
    SetParam("filter_width", static_cast<int32_t>(5));
}

bool FilterChanOp::Cook(const CookContext& context) {
    const ChannelBuffer* inBuf = GetInputBuffer(0);
    if (!inBuf || inBuf->IsEmpty()) {
        SetOutputBuffer(ChannelBuffer{});
        return true;
    }

    // Check reset input
    const ChannelBuffer* resetBuf = GetInputBuffer(1);
    bool doReset = (resetBuf && !resetBuf->IsEmpty() && resetBuf->GetSample(0, 0) > 0.5f);
    if (doReset) {
        m_states.clear();
    }

    int32_t type = GetParam("type").Is<int32_t>() ? GetParam("type").Get<int32_t>() : 0;
    float lagUp = std::max(0.0001f, GetParam("lag_up").Is<float>() ? GetParam("lag_up").Get<float>() : 0.2f);
    float lagDown = std::max(0.0001f, GetParam("lag_down").Is<float>() ? GetParam("lag_down").Get<float>() : 0.2f);
    float springFreq = std::max(0.01f, GetParam("frequency").Is<float>() ? GetParam("frequency").Get<float>() : 2.0f);
    float damping = std::max(0.01f, GetParam("damping").Is<float>() ? GetParam("damping").Get<float>() : 0.707f);
    int32_t filterWidth = std::max(1, GetParam("filter_width").Is<int32_t>() ? GetParam("filter_width").Get<int32_t>() : 5);

    ChannelBuffer outBuf = *inBuf;
    size_t sampleCount = inBuf->GetSampleCount();
    float sampleRate = inBuf->GetSampleRate();
    float dt = (sampleRate > 1e-6f) ? (1.0f / sampleRate) : static_cast<float>(context.deltaTimeSeconds);

    for (size_t c = 0; c < inBuf->GetChannelCount(); ++c) {
        const std::string& name = inBuf->GetChannelNames()[c];
        const float* src = inBuf->GetChannelData(c);
        float* dst = outBuf.GetChannelData(c);

        auto& state = m_states[name];
        if (!state.initialized || doReset) {
            state.value = src[0];
            state.velocity = 0.0f;
            state.initialized = true;
        }

        for (size_t s = 0; s < sampleCount; ++s) {
            float target = src[s];

            if (type == 0) { // Lag (Exponential smoothing)
                float tau = (target >= state.value) ? lagUp : lagDown;
                float alpha = 1.0f - std::exp(-dt / tau);
                alpha = std::clamp(alpha, 0.0f, 1.0f);
                state.value += alpha * (target - state.value);
                dst[s] = state.value;
            } else if (type == 1) { // Spring (2nd-order ODE integration)
                float omega = 2.0f * static_cast<float>(std::numbers::pi) * springFreq;
                float fSpring = -omega * omega * (state.value - target);
                float fDamping = -2.0f * damping * omega * state.velocity;
                float accel = fSpring + fDamping;

                state.velocity += accel * dt;
                state.value += state.velocity * dt;
                dst[s] = state.value;
            } else if (type == 2) { // Moving average over samples
                int32_t halfW = filterWidth / 2;
                float sum = 0.0f;
                int32_t count = 0;
                for (int32_t k = -halfW; k <= halfW; ++k) {
                    int32_t idx = static_cast<int32_t>(s) + k;
                    if (idx >= 0 && idx < static_cast<int32_t>(sampleCount)) {
                        sum += src[idx];
                        count++;
                    }
                }
                dst[s] = (count > 0) ? (sum / static_cast<float>(count)) : target;
            }
        }
    }

    SetOutputBuffer(outBuf);
    return true;
}

} // namespace nf
