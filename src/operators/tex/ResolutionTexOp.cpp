#include "ResolutionTexOp.h"
#include "../../gpu/ShaderCompiler.h"
#include "../../gpu/Device.h"
#include <algorithm>

namespace nf {

struct ResolutionPushConstants {
    glm::vec2 uvScale;
    glm::vec2 uvOffset;
};

ResolutionTexOp::ResolutionTexOp(NodeId id, const std::string& name)
    : TexOp(id, name, "ResolutionTexOp") {
    m_inPin = AddInputPin("input", PinType::Tex);
    m_outPin = AddOutputPin("output", PinType::Tex);

    m_params.AddInt("resolution_w", "Resolution W", 1920, 1, 8192, true);
    m_params.AddInt("resolution_h", "Resolution H", 1080, 1, 8192, true);
    m_params.AddInt("fit_mode", "Fit Mode", 0, 0, 4, true);
}

bool ResolutionTexOp::Cook(const CookContext& context) {
    auto inTex = GetInputTexture(0);
    if (!inTex) {
        SetOutputTexture(nullptr);
        return true;
    }

    int32_t targetW = std::max(1, GetParam("resolution_w").Is<int32_t>() ? GetParam("resolution_w").Get<int32_t>() : 1920);
    int32_t targetH = std::max(1, GetParam("resolution_h").Is<int32_t>() ? GetParam("resolution_h").Get<int32_t>() : 1080);
    int32_t fitMode = GetParam("fit_mode").Is<int32_t>() ? GetParam("fit_mode").Get<int32_t>() : 0;

    ResolutionPushConstants pc{ glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 0.0f) };

    float inAspect = static_cast<float>(inTex->GetWidth()) / static_cast<float>(inTex->GetHeight());
    float outAspect = static_cast<float>(targetW) / static_cast<float>(targetH);

    if (fitMode == 0) {
        // Stretch
        pc.uvScale = glm::vec2(1.0f, 1.0f);
        pc.uvOffset = glm::vec2(0.0f, 0.0f);
    } else if (fitMode == 1) {
        // Fit Horizontal
        float scaleY = outAspect / inAspect;
        pc.uvScale = glm::vec2(1.0f, scaleY);
        pc.uvOffset = glm::vec2(0.0f, (1.0f - scaleY) * 0.5f);
    } else if (fitMode == 2) {
        // Fit Vertical
        float scaleX = inAspect / outAspect;
        pc.uvScale = glm::vec2(scaleX, 1.0f);
        pc.uvOffset = glm::vec2((1.0f - scaleX) * 0.5f, 0.0f);
    } else if (fitMode == 3) {
        // Fill / Crop
        if (inAspect > outAspect) {
            float scaleX = outAspect / inAspect;
            pc.uvScale = glm::vec2(scaleX, 1.0f);
            pc.uvOffset = glm::vec2((1.0f - scaleX) * 0.5f, 0.0f);
        } else {
            float scaleY = inAspect / outAspect;
            pc.uvScale = glm::vec2(1.0f, scaleY);
            pc.uvOffset = glm::vec2(0.0f, (1.0f - scaleY) * 0.5f);
        }
    }

    if (context.gpuDevice && context.commandBuffer && context.texturePool) {
        if (!m_pass) {
            m_pass = std::make_unique<render::FullscreenPass>(context.gpuDevice);
            if (!m_pass->Initialize(gpu::ShaderCompiler::GetResolutionFragmentShader(), 1, sizeof(ResolutionPushConstants))) {
                return false;
            }
        }

        auto target = context.texturePool->Acquire(targetW, targetH, inTex->GetFormat(), VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, context.frameIndex);
        if (!target) return false;

        m_pass->Record(context.commandBuffer, target.get(), { inTex.get() }, &pc, sizeof(ResolutionPushConstants));
        SetOutputTexture(target);
        return true;
    }

    SetOutputTexture(inTex);
    return true;
}

} // namespace nf
