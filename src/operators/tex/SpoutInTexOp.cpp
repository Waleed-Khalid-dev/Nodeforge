#include "SpoutInTexOp.h"

namespace nf {

SpoutInTexOp::SpoutInTexOp(NodeId id, const std::string& name)
    : TexOp(id, name, "SpoutInTexOp") {
    m_outPin = AddOutputPin("output", PinType::Tex);

    SetParam("sender_name", std::string("SpoutSender"));
    SetParam("auto_connect", true);
}

bool SpoutInTexOp::Cook(const CookContext& context) {
    if (context.gpuDevice) {
        uint32_t w = 256;
        uint32_t h = 256;
        std::vector<uint8_t> pixels(w * h * 4, 120);
        auto tex = std::make_shared<gpu::Texture2D>(context.gpuDevice);
        if (tex->UploadData(pixels.data(), w, h, pixels.size())) {
            SetOutputTexture(tex);
        }
    }
    return true;
}

} // namespace nf
