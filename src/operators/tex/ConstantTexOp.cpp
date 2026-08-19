#include "ConstantTexOp.h"
#include "../../gpu/ShaderCompiler.h"
#include "../../gpu/Device.h"
#include <algorithm>

namespace nf {

ConstantTexOp::ConstantTexOp(NodeId id, const std::string& name)
    : TexOp(id, name, "ConstantTexOp") {
    m_outPin = AddOutputPin("output", PinType::Tex);

    m_params.AddInt("resolution_w", "Resolution W", 1280, 1, 8192, true);
    m_params.AddInt("resolution_h", "Resolution H", 720, 1, 8192, true);
    m_params.AddVec4("color", "Color", glm::vec4(0.2f, 0.4f, 0.8f, 1.0f));
}

bool ConstantTexOp::Cook(const CookContext& context) {
    int32_t w = std::max(1, GetParam("resolution_w").Is<int32_t>() ? GetParam("resolution_w").Get<int32_t>() : 1280);
    int32_t h = std::max(1, GetParam("resolution_h").Is<int32_t>() ? GetParam("resolution_h").Get<int32_t>() : 720);
    glm::vec4 color = GetParam("color").Is<glm::vec4>() ? GetParam("color").Get<glm::vec4>() : glm::vec4(0.2f, 0.4f, 0.8f, 1.0f);

    if (context.gpuDevice && context.commandBuffer && context.texturePool) {
        if (!m_pass) {
            m_pass = std::make_unique<render::FullscreenPass>(context.gpuDevice);
            if (!m_pass->Initialize(gpu::ShaderCompiler::GetConstantFragmentShader(), 0, sizeof(glm::vec4))) {
                return false;
            }
        }

        auto target = context.texturePool->Acquire(w, h, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, context.frameIndex);
        if (!target) return false;

        m_pass->Record(context.commandBuffer, target.get(), {}, &color, sizeof(glm::vec4));
        SetOutputTexture(target);
        return true;
    }

    if (context.gpuDevice) {
        auto tex = std::make_unique<gpu::Texture2D>(context.gpuDevice);
        uint8_t r = static_cast<uint8_t>(std::clamp(color.r * 255.0f, 0.0f, 255.0f));
        uint8_t g = static_cast<uint8_t>(std::clamp(color.g * 255.0f, 0.0f, 255.0f));
        uint8_t b = static_cast<uint8_t>(std::clamp(color.b * 255.0f, 0.0f, 255.0f));
        uint8_t a = static_cast<uint8_t>(std::clamp(color.a * 255.0f, 0.0f, 255.0f));

        std::vector<uint8_t> pixels(w * h * 4);
        for (size_t i = 0; i < pixels.size(); i += 4) {
            pixels[i + 0] = r;
            pixels[i + 1] = g;
            pixels[i + 2] = b;
            pixels[i + 3] = a;
        }

        if (tex->UploadData(pixels.data(), w, h, pixels.size())) {
            SetOutputTexture(std::shared_ptr<gpu::Texture2D>(tex.release()));
            return true;
        }
    }

    return true;
}

} // namespace nf
