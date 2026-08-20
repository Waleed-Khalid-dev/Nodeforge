#include "VideoDeviceInTexOp.h"

namespace nf {

VideoDeviceInTexOp::VideoDeviceInTexOp(NodeId id, const std::string& name)
    : TexOp(id, name, "VideoDeviceInTexOp") {
    m_outPin = AddOutputPin("output", PinType::Tex);

    SetParam("device_index", static_cast<int32_t>(0));
    SetParam("resolution", glm::vec2(1920.0f, 1080.0f));
    SetParam("framerate", 60.0f);
    SetParam("active", true);
}

bool VideoDeviceInTexOp::Cook(const CookContext& context) {
    bool active = GetParam("active").Is<bool>() ? GetParam("active").Get<bool>() : true;
    if (!active) {
        return true;
    }

    // Video capture frame synthesis for testing / runtime ingestion
    if (context.gpuDevice) {
        uint32_t w = 320;
        uint32_t h = 240;
        std::vector<uint8_t> pixels(w * h * 4, 180);
        auto tex = std::make_shared<gpu::Texture2D>(context.gpuDevice);
        if (tex->UploadData(pixels.data(), w, h, pixels.size())) {
            SetOutputTexture(tex);
        }
    }

    return true;
}

} // namespace nf
