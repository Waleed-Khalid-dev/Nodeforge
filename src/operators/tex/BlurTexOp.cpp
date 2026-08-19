#include "BlurTexOp.h"
#include "../../gpu/ShaderCompiler.h"
#include "../../gpu/Device.h"

namespace nf {

struct BlurPushConstants {
    glm::vec2 direction;
    float radius;
};

BlurTexOp::BlurTexOp(NodeId id, const std::string& name)
    : TexOp(id, name, "BlurTexOp") {
    m_inPin = AddInputPin("input", PinType::Tex);
    m_outPin = AddOutputPin("output", PinType::Tex);

    m_params.AddFloat("radius", "Radius", 4.0f, 0.0f, 64.0f, true);
    m_params.AddInt("passes", "Passes", 1, 1, 4, true);
}

bool BlurTexOp::Cook(const CookContext& context) {
    auto inTex = GetInputTexture(0);
    if (!inTex) {
        SetOutputTexture(nullptr);
        return true;
    }

    float radius = GetParam("radius").Is<float>() ? GetParam("radius").Get<float>() : 4.0f;
    if (radius <= 0.0f) {
        SetOutputTexture(inTex);
        return true;
    }

    if (context.gpuDevice && context.commandBuffer && context.texturePool) {
        if (!m_pass) {
            m_pass = std::make_unique<render::FullscreenPass>(context.gpuDevice);
            if (!m_pass->Initialize(gpu::ShaderCompiler::GetBlurFragmentShader(), 1, sizeof(BlurPushConstants))) {
                return false;
            }
        }

        uint32_t w = inTex->GetWidth();
        uint32_t h = inTex->GetHeight();
        VkFormat fmt = inTex->GetFormat();

        auto pingPong = context.texturePool->Acquire(w, h, fmt, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, context.frameIndex);
        auto finalTarget = context.texturePool->Acquire(w, h, fmt, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, context.frameIndex);
        if (!pingPong || !finalTarget) return false;

        // Pass 1: Horizontal Blur
        BlurPushConstants pcH{};
        pcH.direction = glm::vec2(1.0f / static_cast<float>(w), 0.0f);
        pcH.radius = radius;
        m_pass->Record(context.commandBuffer, pingPong.get(), { inTex.get() }, &pcH, sizeof(BlurPushConstants));

        // Pass 2: Vertical Blur
        BlurPushConstants pcV{};
        pcV.direction = glm::vec2(0.0f, 1.0f / static_cast<float>(h));
        pcV.radius = radius;
        m_pass->Record(context.commandBuffer, finalTarget.get(), { pingPong.get() }, &pcV, sizeof(BlurPushConstants));

        SetOutputTexture(finalTarget);
        return true;
    }

    SetOutputTexture(inTex);
    return true;
}

} // namespace nf
