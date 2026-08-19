#include "TrailChanOp.h"
#include <algorithm>

namespace nf {

TrailChanOp::TrailChanOp(NodeId id, const std::string& name)
    : ChanOp(id, name, "TrailChanOp") {
    AddInputPin("input", PinType::Chan);
    AddInputPin("reset", PinType::Chan);
    m_outPin = AddOutputPin("output", PinType::Chan);

    SetParam("length", 4.0f); // seconds
    SetParam("max_samples", static_cast<int32_t>(240));
    SetParam("capture", true);
}

bool TrailChanOp::Cook(const CookContext& /*context*/) {
    const ChannelBuffer* inBuf = GetInputBuffer(0);
    const ChannelBuffer* resetBuf = GetInputBuffer(1);

    bool doReset = (resetBuf && !resetBuf->IsEmpty() && resetBuf->GetSample(0, 0) > 0.5f);
    if (doReset) {
        m_history.clear();
    }

    if (!inBuf || inBuf->IsEmpty()) {
        SetOutputBuffer(ChannelBuffer{});
        return true;
    }

    bool capture = GetParam("capture").Is<bool>() ? GetParam("capture").Get<bool>() : true;
    int32_t maxSamplesParam = GetParam("max_samples").Is<int32_t>() ? GetParam("max_samples").Get<int32_t>() : 240;
    float lengthSec = GetParam("length").Is<float>() ? GetParam("length").Get<float>() : 4.0f;
    float rate = inBuf->GetSampleRate();
    size_t targetHistorySize = static_cast<size_t>(std::max(10, std::min(maxSamplesParam, static_cast<int32_t>(lengthSec * rate))));

    if (capture) {
        for (size_t c = 0; c < inBuf->GetChannelCount(); ++c) {
            const std::string& name = inBuf->GetChannelNames()[c];
            const float* src = inBuf->GetChannelData(c);
            size_t count = inBuf->GetSampleCount();

            auto& deq = m_history[name];
            for (size_t s = 0; s < count; ++s) {
                deq.push_back(src[s]);
            }
            while (deq.size() > targetHistorySize) {
                deq.pop_front();
            }
        }
    }

    std::vector<std::string> names = inBuf->GetChannelNames();
    size_t recordedCount = 0;
    for (const auto& name : names) {
        recordedCount = std::max(recordedCount, m_history[name].size());
    }

    if (recordedCount == 0) {
        SetOutputBuffer(*inBuf);
        return true;
    }

    ChannelBuffer outBuf(names, recordedCount, rate, inBuf->GetStartSample());

    for (size_t c = 0; c < names.size(); ++c) {
        const auto& deq = m_history[names[c]];
        float* dst = outBuf.GetChannelData(c);
        if (dst && !deq.empty()) {
            size_t offset = recordedCount - deq.size();
            for (size_t i = 0; i < offset; ++i) {
                dst[i] = deq.front();
            }
            for (size_t i = 0; i < deq.size(); ++i) {
                dst[offset + i] = deq[i];
            }
        }
    }

    SetOutputBuffer(outBuf);
    return true;
}

} // namespace nf
