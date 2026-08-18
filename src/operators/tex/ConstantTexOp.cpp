#include "ConstantTexOp.h"
#include "../../gpu/Device.h"
#include <algorithm>

namespace nf {

ConstantTexOp::ConstantTexOp(NodeId id, const std::string& name)
    : Node(id, name, "ConstantTexOp") {
    m_outPin = AddOutputPin("output", PinType::Tex);

    SetParam("resolution_w", static_cast<int32_t>(256));
    SetParam("resolution_h", static_cast<int32_t>(256));
    SetParam("color", glm::vec4(0.2f, 0.4f, 0.8f, 1.0f));
}

bool ConstantTexOp::Cook(const CookContext& context) {
    int32_t w = 256;
    if (GetParam("resolution_w").Is<int32_t>()) w = std::max(1, GetParam("resolution_w").Get<int32_t>());
    int32_t h = 256;
    if (GetParam("resolution_h").Is<int32_t>()) h = std::max(1, GetParam("resolution_h").Get<int32_t>());

    glm::vec4 color(0.2f, 0.4f, 0.8f, 1.0f);
    if (GetParam("color").Is<glm::vec4>()) color = GetParam("color").Get<glm::vec4>();

    if (context.gpuDevice) {
        if (!m_texture) {
            m_texture = std::make_shared<gpu::Texture2D>(context.gpuDevice);
        }

        uint8_t r = static_cast<uint8_t>(std::clamp(color.r, 0.0f, 1.0f) * 255.0f);
        uint8_t g = static_cast<uint8_t>(std::clamp(color.g, 0.0f, 1.0f) * 255.0f);
        uint8_t b = static_cast<uint8_t>(std::clamp(color.b, 0.0f, 1.0f) * 255.0f);
        uint8_t a = static_cast<uint8_t>(std::clamp(color.a, 0.0f, 1.0f) * 255.0f);

        size_t totalBytes = static_cast<size_t>(w * h * 4);
        std::vector<uint8_t> pixels(totalBytes);
        for (size_t i = 0; i < totalBytes; i += 4) {
            pixels[i + 0] = r;
            pixels[i + 1] = g;
            pixels[i + 2] = b;
            pixels[i + 3] = a;
        }

        m_texture->UploadData(pixels.data(), static_cast<uint32_t>(w), static_cast<uint32_t>(h), totalBytes);
        m_outPin->SetValue(PinValue(m_texture));
    } else {
        // Headless / mock mode
        m_outPin->SetValue(PinValue(std::shared_ptr<gpu::Texture2D>(nullptr)));
    }

    return true;
}

} // namespace nf
