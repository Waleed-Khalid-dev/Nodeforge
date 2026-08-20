#include "NDIInTexOp.h"

namespace nf {

NDIInTexOp::NDIInTexOp(NodeId id, const std::string& name)
    : TexOp(id, name, "NDIInTexOp") {
    m_outPin = AddOutputPin("output", PinType::Tex);

    SetParam("source_name", std::string("NDISource"));
    SetParam("bandwidth", static_cast<int32_t>(0));
    SetParam("active", true);
}

bool NDIInTexOp::Cook(const CookContext& context) {
    if (context.gpuDevice) {
        uint32_t w = 256;
        uint32_t h = 256;
        std::vector<uint8_t> pixels(w * h * 4, 100);
        auto tex = std::make_shared<gpu::Texture2D>(context.gpuDevice);
        if (tex->UploadData(pixels.data(), w, h, pixels.size())) {
            SetOutputTexture(tex);
        }
    }
    return true;
}

} // namespace nf
