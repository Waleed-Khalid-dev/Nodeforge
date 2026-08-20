#include "WarpBlendTexOp.h"

namespace nf {

WarpBlendTexOp::WarpBlendTexOp(NodeId id, const std::string& name)
    : TexOp(id, name, "WarpBlendTexOp"), m_warpMesh(4, 4) {
    m_inPin = AddInputPin("input", PinType::Tex);
    m_maskPin = AddInputPin("mask", PinType::Tex);
    m_outPin = AddOutputPin("output", PinType::Tex);

    SetParam("grid_rows", static_cast<int32_t>(4));
    SetParam("grid_cols", static_cast<int32_t>(4));
    SetParam("blend_left", 0.0f);
    SetParam("blend_right", 0.0f);
    SetParam("blend_top", 0.0f);
    SetParam("blend_bottom", 0.0f);
    SetParam("blend_gamma", 2.2f);
    SetParam("black_level", 0.0f);
    SetParam("test_pattern", static_cast<int32_t>(0)); // 0: Disabled, 1: Grid, 2: Circles, 3: SMPTE, 4: Checkerboard
    SetParam("calibration_mode", false);
}

bool WarpBlendTexOp::Cook(const CookContext& context) {
    int32_t rows = GetParam("grid_rows").Is<int32_t>() ? GetParam("grid_rows").Get<int32_t>() : 4;
    int32_t cols = GetParam("grid_cols").Is<int32_t>() ? GetParam("grid_cols").Get<int32_t>() : 4;

    if (m_warpMesh.GetRows() != rows || m_warpMesh.GetCols() != cols) {
        m_warpMesh.SetGridSize(rows, cols);
    }

    float bLeft = GetParam("blend_left").Is<float>() ? GetParam("blend_left").Get<float>() : 0.0f;
    float bRight = GetParam("blend_right").Is<float>() ? GetParam("blend_right").Get<float>() : 0.0f;
    float bTop = GetParam("blend_top").Is<float>() ? GetParam("blend_top").Get<float>() : 0.0f;
    float bBottom = GetParam("blend_bottom").Is<float>() ? GetParam("blend_bottom").Get<float>() : 0.0f;
    float gamma = GetParam("blend_gamma").Is<float>() ? GetParam("blend_gamma").Get<float>() : 2.2f;
    float blackLevel = GetParam("black_level").Is<float>() ? GetParam("black_level").Get<float>() : 0.0f;
    int32_t testPattern = GetParam("test_pattern").Is<int32_t>() ? GetParam("test_pattern").Get<int32_t>() : 0;

    auto inTex = GetInputTexture(0);

    if (context.gpuDevice && context.texturePool) {
        if (!m_pass) {
            m_pass = std::make_unique<WarpBlendPass>(context.gpuDevice, context.texturePool);
        }

        uint32_t w = inTex ? inTex->GetWidth() : 1920;
        uint32_t h = inTex ? inTex->GetHeight() : 1080;

        auto targetTex = context.texturePool->Acquire(w, h, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, context.frameIndex);
        if (targetTex) {
            glm::vec4 blendEdges(bLeft, bRight, bTop, bBottom);
            m_pass->Execute(targetTex.get(), inTex.get(), m_warpMesh, blendEdges, gamma, blackLevel, testPattern);
            SetOutputTexture(targetTex);
        }
    } else if (inTex) {
        SetOutputTexture(inTex);
    }

    return true;
}

} // namespace nf
