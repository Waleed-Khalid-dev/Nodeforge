#include "TransformTexOp.h"
#include "../../gpu/ShaderCompiler.h"
#include "../../gpu/Device.h"
#include <glm/gtc/constants.hpp>
#include <cmath>

namespace nf {

struct TransformPushConstants {
    glm::vec2 translate;
    glm::vec2 scale;
    glm::vec2 pivot;
    float rotate; // in radians
};

TransformTexOp::TransformTexOp(NodeId id, const std::string& name)
    : TexOp(id, name, "TransformTexOp") {
    m_inPin = AddInputPin("input", PinType::Tex);
    m_outPin = AddOutputPin("output", PinType::Tex);

    m_params.AddVec2("translate", "Translate", glm::vec2(0.0f, 0.0f));
    m_params.AddFloat("rotate", "Rotate", 0.0f, -360.0f, 360.0f, true);
    m_params.AddVec2("scale", "Scale", glm::vec2(1.0f, 1.0f));
    m_params.AddVec2("pivot", "Pivot", glm::vec2(0.5f, 0.5f));
}

bool TransformTexOp::Cook(const CookContext& context) {
    auto inTex = GetInputTexture(0);
    if (!inTex) {
        SetOutputTexture(nullptr);
        return true;
    }

    TransformPushConstants pc{};
    pc.translate = GetParam("translate").Is<glm::vec2>() ? GetParam("translate").Get<glm::vec2>() : glm::vec2(0.0f, 0.0f);
    float deg = GetParam("rotate").Is<float>() ? GetParam("rotate").Get<float>() : 0.0f;
    pc.rotate = glm::radians(deg);
    pc.scale = GetParam("scale").Is<glm::vec2>() ? GetParam("scale").Get<glm::vec2>() : glm::vec2(1.0f, 1.0f);
    pc.pivot = GetParam("pivot").Is<glm::vec2>() ? GetParam("pivot").Get<glm::vec2>() : glm::vec2(0.5f, 0.5f);

    if (context.gpuDevice && context.commandBuffer && context.texturePool) {
        if (!m_pass) {
            m_pass = std::make_unique<render::FullscreenPass>(context.gpuDevice);
            if (!m_pass->Initialize(gpu::ShaderCompiler::GetTransformFragmentShader(), 1, sizeof(TransformPushConstants))) {
                return false;
            }
        }

        uint32_t w = inTex->GetWidth();
        uint32_t h = inTex->GetHeight();
        auto target = context.texturePool->Acquire(w, h, inTex->GetFormat(), VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, context.frameIndex);
        if (!target) return false;

        m_pass->Record(context.commandBuffer, target.get(), { inTex.get() }, &pc, sizeof(TransformPushConstants));
        SetOutputTexture(target);
        return true;
    }

    SetOutputTexture(inTex);
    return true;
}

} // namespace nf
