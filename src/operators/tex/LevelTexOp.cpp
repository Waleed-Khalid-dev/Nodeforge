#include "LevelTexOp.h"
#include "../../gpu/ShaderCompiler.h"
#include "../../gpu/Device.h"

namespace nf {

struct LevelPushConstants {
    float blackLevel;
    float whiteLevel;
    float brightness;
    float gamma;
    float contrast;
};

LevelTexOp::LevelTexOp(NodeId id, const std::string& name)
    : TexOp(id, name, "LevelTexOp") {
    m_inPin = AddInputPin("input", PinType::Tex);
    m_outPin = AddOutputPin("output", PinType::Tex);

    m_params.AddFloat("black_level", "Black Level", 0.0f, 0.0f, 1.0f, true);
    m_params.AddFloat("white_level", "White Level", 1.0f, 0.0f, 1.0f, true);
    m_params.AddFloat("brightness", "Brightness", 1.0f, 0.0f, 4.0f, true);
    m_params.AddFloat("gamma", "Gamma", 1.0f, 0.1f, 5.0f, true);
    m_params.AddFloat("contrast", "Contrast", 1.0f, 0.0f, 4.0f, true);
}

bool LevelTexOp::Cook(const CookContext& context) {
    auto inTex = GetInputTexture(0);
    if (!inTex) {
        SetOutputTexture(nullptr);
        return true;
    }

    LevelPushConstants pc{};
    pc.blackLevel = GetParam("black_level").Is<float>() ? GetParam("black_level").Get<float>() : 0.0f;
    pc.whiteLevel = GetParam("white_level").Is<float>() ? GetParam("white_level").Get<float>() : 1.0f;
    pc.brightness = GetParam("brightness").Is<float>() ? GetParam("brightness").Get<float>() : 1.0f;
    pc.gamma = GetParam("gamma").Is<float>() ? GetParam("gamma").Get<float>() : 1.0f;
    pc.contrast = GetParam("contrast").Is<float>() ? GetParam("contrast").Get<float>() : 1.0f;

    if (context.gpuDevice && context.commandBuffer && context.texturePool) {
        if (!m_pass) {
            m_pass = std::make_unique<render::FullscreenPass>(context.gpuDevice);
            if (!m_pass->Initialize(gpu::ShaderCompiler::GetLevelFragmentShader(), 1, sizeof(LevelPushConstants))) {
                return false;
            }
        }

        uint32_t w = inTex->GetWidth();
        uint32_t h = inTex->GetHeight();
        auto target = context.texturePool->Acquire(w, h, inTex->GetFormat(), VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, context.frameIndex);
        if (!target) return false;

        m_pass->Record(context.commandBuffer, target.get(), { inTex.get() }, &pc, sizeof(LevelPushConstants));
        SetOutputTexture(target);
        return true;
    }

    SetOutputTexture(inTex);
    return true;
}

} // namespace nf
