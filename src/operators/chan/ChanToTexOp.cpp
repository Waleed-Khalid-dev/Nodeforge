#include "ChanToTexOp.h"
#include "../../core/ChannelBuffer.h"
#include <algorithm>

namespace nf {

ChanToTexOp::ChanToTexOp(NodeId id, const std::string& name)
    : TexOp(id, name, "ChanToTexOp") {
    m_inPin = AddInputPin("input", PinType::Chan);
    m_outPin = AddOutputPin("output", PinType::Tex);

    m_params.AddInt("data_format", "Data Format", 0, 0, 2); // 0: RGBA8, 1: R8, 2: Float
    m_params.AddBool("normalize", "Normalize", false);
}

bool ChanToTexOp::Cook(const CookContext& context) {
    if (!m_inPin || !m_inPin->GetValue().Is<ChannelBuffer>()) {
        SetOutputTexture(nullptr);
        return true;
    }

    const ChannelBuffer& buf = m_inPin->GetValue().Get<ChannelBuffer>();
    if (buf.IsEmpty()) {
        SetOutputTexture(nullptr);
        return true;
    }

    uint32_t width = static_cast<uint32_t>(std::max(1ULL, buf.GetSampleCount()));
    uint32_t height = static_cast<uint32_t>(std::max(1ULL, buf.GetChannelCount()));

    std::vector<uint8_t> pixels(width * height * 4, 255);

    for (size_t c = 0; c < buf.GetChannelCount(); ++c) {
        const float* chanData = buf.GetChannelData(c);
        for (size_t s = 0; s < buf.GetSampleCount(); ++s) {
            float val = chanData[s];
            uint8_t byteVal = static_cast<uint8_t>(std::clamp(val * 255.0f, 0.0f, 255.0f));
            size_t pixelIdx = (c * width + s) * 4;
            if (pixelIdx + 3 < pixels.size()) {
                pixels[pixelIdx + 0] = byteVal;
                pixels[pixelIdx + 1] = byteVal;
                pixels[pixelIdx + 2] = byteVal;
                pixels[pixelIdx + 3] = 255;
            }
        }
    }

    if (context.gpuDevice) {
        auto tex = std::make_unique<gpu::Texture2D>(context.gpuDevice);
        if (tex->UploadData(pixels.data(), width, height, pixels.size())) {
            SetOutputTexture(std::shared_ptr<gpu::Texture2D>(tex.release()));
            return true;
        }
    }

    return true;
}

} // namespace nf
