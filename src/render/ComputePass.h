#pragma once

#include "../gpu/Device.h"
#include "../gpu/Texture.h"
#include "../gpu/ShaderCompiler.h"
#include <string>

namespace render {

class ComputePass {
public:
    ComputePass(gpu::Device* device);
    ~ComputePass();

    ComputePass(const ComputePass&) = delete;
    ComputePass& operator=(const ComputePass&) = delete;

    bool Initialize(const std::string& compShaderGlsl, size_t pushConstantSize = 0);
    void Cleanup();

    void Dispatch(VkCommandBuffer cmd, gpu::Texture2D* outputImage, uint32_t groupCountX, uint32_t groupCountY, const void* pushConstantData = nullptr, size_t pushConstantSize = 0);

private:
    gpu::Device* m_device = nullptr;
    VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
    VkDescriptorSet m_descriptorSet = VK_NULL_HANDLE;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkPipeline m_pipeline = VK_NULL_HANDLE;
};

} // namespace render
