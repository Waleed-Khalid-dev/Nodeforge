#pragma once

#include "../gpu/Device.h"
#include "../gpu/Texture.h"
#include "../gpu/ShaderCompiler.h"
#include <vector>
#include <string>

namespace render {

class FullscreenPass {
public:
    FullscreenPass(gpu::Device* device);
    ~FullscreenPass();

    FullscreenPass(const FullscreenPass&) = delete;
    FullscreenPass& operator=(const FullscreenPass&) = delete;

    bool Initialize(const std::string& fragShaderGlsl, uint32_t numSamplers = 0, size_t pushConstantSize = 0, VkFormat targetFormat = VK_FORMAT_R8G8B8A8_UNORM);
    void Cleanup();

    void Record(VkCommandBuffer cmd, gpu::Texture2D* target, const std::vector<gpu::Texture2D*>& inputSamplers, const void* pushConstantData = nullptr, size_t pushConstantSize = 0);

private:
    gpu::Device* m_device = nullptr;
    VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
    VkDescriptorSet m_descriptorSet = VK_NULL_HANDLE;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkPipeline m_pipeline = VK_NULL_HANDLE;
    uint32_t m_numSamplers = 0;
};

} // namespace render
