#include "LoadImageTexOp.h"
#include "../../gpu/Device.h"
#include <spdlog/spdlog.h>
#include <stb_image.h>
#include <filesystem>

namespace nf {

LoadImageTexOp::LoadImageTexOp(NodeId id, const std::string& name)
    : TexOp(id, name, "LoadImageTexOp") {
    m_outPin = AddOutputPin("output", PinType::Tex);

    m_params.AddString("file_path", "File Path", "");
    m_params.AddBool("reload", "Reload", false);
}

bool LoadImageTexOp::Cook(const CookContext& context) {
    if (!context.gpuDevice) return true;

    std::string filePath = GetParam("file_path").Is<std::string>() ? GetParam("file_path").Get<std::string>() : "";
    bool reload = GetParam("reload").Is<bool>() ? GetParam("reload").Get<bool>() : false;

    if (filePath.empty()) {
        if (!m_outputTexture) {
            auto tex = std::make_unique<gpu::Texture2D>(context.gpuDevice);
            tex->CreateProceduralCheckerboard();
            SetOutputTexture(std::shared_ptr<gpu::Texture2D>(tex.release()));
        }
        return true;
    }

    if (m_outputTexture && filePath == m_lastLoadedPath && !reload) {
        return true;
    }

    int width = 0, height = 0, channels = 0;
    stbi_uc* data = stbi_load(filePath.c_str(), &width, &height, &channels, STBI_rgb_alpha);
    if (!data) {
        spdlog::warn("LoadImageTexOp failed to load image '{}': {}", filePath, stbi_failure_reason());
        return false;
    }

    auto tex = std::make_unique<gpu::Texture2D>(context.gpuDevice);
    VkDeviceSize size = static_cast<VkDeviceSize>(width) * height * 4;
    bool ok = tex->UploadData(data, static_cast<uint32_t>(width), static_cast<uint32_t>(height), size);
    stbi_image_free(data);

    if (ok) {
        m_lastLoadedPath = filePath;
        SetOutputTexture(std::shared_ptr<gpu::Texture2D>(tex.release()));
        return true;
    }

    return false;
}

} // namespace nf
