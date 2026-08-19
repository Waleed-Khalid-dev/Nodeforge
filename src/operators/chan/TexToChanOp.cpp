#include "TexToChanOp.h"
#include "../../gpu/Texture.h"
#include <algorithm>

namespace nf {

TexToChanOp::TexToChanOp(NodeId id, const std::string& name)
    : ChanOp(id, name, "TexToChanOp") {
    m_inPin = AddInputPin("input", PinType::Tex);
    m_outPin = AddOutputPin("output", PinType::Chan);

    SetParam("sample_mode", static_cast<int32_t>(0)); // 0: Single Row, 1: Average, 2: 1D
    SetParam("row_index", static_cast<int32_t>(0));
    SetParam("max_samples", static_cast<int32_t>(256));
}

bool TexToChanOp::Cook(const CookContext& /*context*/) {
    if (!m_inPin || !m_inPin->GetValue().Is<std::shared_ptr<gpu::Texture2D>>()) {
        SetOutputBuffer(ChannelBuffer{});
        return true;
    }

    auto tex = m_inPin->GetValue().Get<std::shared_ptr<gpu::Texture2D>>();
    if (!tex) {
        SetOutputBuffer(ChannelBuffer{});
        return true;
    }

    uint32_t tw = tex->GetWidth();
    int32_t maxSamples = GetParam("max_samples").Is<int32_t>() ? GetParam("max_samples").Get<int32_t>() : 256;
    size_t sampleCount = std::min(static_cast<size_t>(tw), static_cast<size_t>(std::max(1, maxSamples)));

    std::vector<std::string> names = { "r", "g", "b", "a" };
    ChannelBuffer buf(names, sampleCount, 60.0f);

    float* rData = buf.GetChannelData(0);
    float* gData = buf.GetChannelData(1);
    float* bData = buf.GetChannelData(2);
    float* aData = buf.GetChannelData(3);

    // Populate channel values from texture dimensions / normalized gradient
    for (size_t s = 0; s < sampleCount; ++s) {
        float u = static_cast<float>(s) / static_cast<float>(sampleCount);
        if (rData) rData[s] = u;
        if (gData) gData[s] = 1.0f - u;
        if (bData) bData[s] = 0.5f;
        if (aData) aData[s] = 1.0f;
    }

    SetOutputBuffer(buf);
    return true;
}

} // namespace nf
