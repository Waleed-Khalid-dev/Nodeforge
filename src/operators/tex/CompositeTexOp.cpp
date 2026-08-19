#include "CompositeTexOp.h"
#include "../../gpu/ShaderCompiler.h"
#include "../../gpu/Device.h"

namespace nf {

struct CompositePushConstants {
    int32_t operation;
    float opacity;
};

CompositeTexOp::CompositeTexOp(NodeId id, const std::string& name)
    : TexOp(id, name, "CompositeTexOp") {
    m_inPin1 = AddInputPin("input1", PinType::Tex);
    m_inPin2 = AddInputPin("input2", PinType::Tex);
    m_outPin = AddOutputPin("output", PinType::Tex);

    m_params.AddInt("operation", "Operation", 0, 0, 6, true);
    m_params.AddFloat("opacity", "Opacity", 1.0f, 0.0f, 1.0f, true);
}

bool CompositeTexOp::Cook(const CookContext& context) {
    auto in1 = GetInputTexture(0);
    auto in2 = GetInputTexture(1);

    if (!in1 && !in2) {
        SetOutputTexture(nullptr);
        return true;
    }

    if (!in1) {
        SetOutputTexture(in2);
        return true;
    }
    if (!in2) {
        SetOutputTexture(in1);
        return true;
    }

    CompositePushConstants pc{};
    pc.operation = GetParam("operation").Is<int32_t>() ? GetParam("operation").Get<int32_t>() : 0;
    pc.opacity = GetParam("opacity").Is<float>() ? GetParam("opacity").Get<float>() : 1.0f;

    if (context.gpuDevice && context.commandBuffer && context.texturePool) {
        if (!m_pass) {
            m_pass = std::make_unique<render::FullscreenPass>(context.gpuDevice);
            if (!m_pass->Initialize(gpu::ShaderCompiler::GetCompositeFragmentShader(), 2, sizeof(CompositePushConstants))) {
                return false;
            }
        }

        uint32_t w = in1->GetWidth();
        uint32_t h = in1->GetHeight();
        auto target = context.texturePool->Acquire(w, h, in1->GetFormat(), VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, context.frameIndex);
        if (!target) return false;

        m_pass->Record(context.commandBuffer, target.get(), { in1.get(), in2.get() }, &pc, sizeof(CompositePushConstants));
        SetOutputTexture(target);
        return true;
    }

    SetOutputTexture(in1);
    return true;
}

} // namespace nf
