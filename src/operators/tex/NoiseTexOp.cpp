#include "NoiseTexOp.h"
#include "../../gpu/ShaderCompiler.h"
#include "../../gpu/Device.h"
#include <algorithm>
#include <cmath>

namespace nf {

struct NoisePushConstants {
    int32_t width;
    int32_t height;
    int32_t noiseType;
    int32_t octaves;
    float period;
    float roughness;
    float tx;
    float ty;
    float tz;
};

NoiseTexOp::NoiseTexOp(NodeId id, const std::string& name)
    : TexOp(id, name, "NoiseTexOp") {
    m_outPin = AddOutputPin("output", PinType::Tex);

    m_params.AddInt("resolution_w", "Resolution W", 1280, 1, 8192, true);
    m_params.AddInt("resolution_h", "Resolution H", 720, 1, 8192, true);
    m_params.AddInt("noise_type", "Noise Type", 0, 0, 2, true);
    m_params.AddFloat("period", "Period", 1.0f, 0.01f, 100.0f, true);
    m_params.AddInt("octaves", "Octaves", 3, 1, 8, true);
    m_params.AddFloat("roughness", "Roughness", 0.5f, 0.0f, 1.0f, true);
    m_params.AddFloat("translate_x", "Translate X", 0.0f);
    m_params.AddFloat("translate_y", "Translate Y", 0.0f);
    m_params.AddFloat("translate_z", "Translate Z", 0.0f);
}

bool NoiseTexOp::Cook(const CookContext& context) {
    int32_t w = std::max(1, GetParam("resolution_w").Is<int32_t>() ? GetParam("resolution_w").Get<int32_t>() : 1280);
    int32_t h = std::max(1, GetParam("resolution_h").Is<int32_t>() ? GetParam("resolution_h").Get<int32_t>() : 720);

    NoisePushConstants pc{};
    pc.width = w;
    pc.height = h;
    pc.noiseType = GetParam("noise_type").Is<int32_t>() ? GetParam("noise_type").Get<int32_t>() : 0;
    pc.period = GetParam("period").Is<float>() ? GetParam("period").Get<float>() : 1.0f;
    pc.octaves = GetParam("octaves").Is<int32_t>() ? GetParam("octaves").Get<int32_t>() : 3;
    pc.roughness = GetParam("roughness").Is<float>() ? GetParam("roughness").Get<float>() : 0.5f;
    pc.tx = GetParam("translate_x").Is<float>() ? GetParam("translate_x").Get<float>() : 0.0f;
    pc.ty = GetParam("translate_y").Is<float>() ? GetParam("translate_y").Get<float>() : 0.0f;
    pc.tz = GetParam("translate_z").Is<float>() ? GetParam("translate_z").Get<float>() : 0.0f;

    if (context.gpuDevice && context.commandBuffer && context.texturePool) {
        if (!m_computePass) {
            m_computePass = std::make_unique<render::ComputePass>(context.gpuDevice);
            if (!m_computePass->Initialize(gpu::ShaderCompiler::GetNoiseComputeShader(), sizeof(NoisePushConstants))) {
                return false;
            }
        }

        auto target = context.texturePool->Acquire(w, h, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, context.frameIndex);
        if (!target) return false;

        uint32_t groupX = (w + 15) / 16;
        uint32_t groupY = (h + 15) / 16;
        m_computePass->Dispatch(context.commandBuffer, target.get(), groupX, groupY, &pc, sizeof(NoisePushConstants));
        SetOutputTexture(target);
        return true;
    }

    if (context.gpuDevice) {
        auto tex = std::make_unique<gpu::Texture2D>(context.gpuDevice);
        std::vector<uint8_t> pixels(w * h * 4);
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                float u = static_cast<float>(x) / static_cast<float>(w);
                float v = static_cast<float>(y) / static_cast<float>(h);
                float n = std::sin(u * pc.period * 6.28f + pc.tx) * std::cos(v * pc.period * 6.28f + pc.ty);
                uint8_t val = static_cast<uint8_t>(std::clamp((n * 0.5f + 0.5f) * 255.0f, 0.0f, 255.0f));
                size_t idx = (y * w + x) * 4;
                pixels[idx + 0] = val;
                pixels[idx + 1] = val;
                pixels[idx + 2] = val;
                pixels[idx + 3] = 255;
            }
        }
        if (tex->UploadData(pixels.data(), w, h, pixels.size())) {
            SetOutputTexture(std::shared_ptr<gpu::Texture2D>(tex.release()));
            return true;
        }
    }

    return true;
}

} // namespace nf
