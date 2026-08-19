#include "ComputePass.h"
#include <spdlog/spdlog.h>

namespace render {

ComputePass::ComputePass(gpu::Device* device) : m_device(device) {
}

ComputePass::~ComputePass() {
    Cleanup();
}

void ComputePass::Cleanup() {
    if (!m_device || !m_device->GetDevice()) return;
    VkDevice dev = m_device->GetDevice();

    if (m_pipeline) {
        vkDestroyPipeline(dev, m_pipeline, nullptr);
        m_pipeline = VK_NULL_HANDLE;
    }
    if (m_pipelineLayout) {
        vkDestroyPipelineLayout(dev, m_pipelineLayout, nullptr);
        m_pipelineLayout = VK_NULL_HANDLE;
    }
    if (m_descriptorPool) {
        vkDestroyDescriptorPool(dev, m_descriptorPool, nullptr);
        m_descriptorPool = VK_NULL_HANDLE;
    }
    if (m_descriptorSetLayout) {
        vkDestroyDescriptorSetLayout(dev, m_descriptorSetLayout, nullptr);
        m_descriptorSetLayout = VK_NULL_HANDLE;
    }
    m_descriptorSet = VK_NULL_HANDLE;
}

bool ComputePass::Initialize(const std::string& compShaderGlsl, size_t pushConstantSize) {
    Cleanup();
    VkDevice dev = m_device->GetDevice();

    std::string err;
    VkShaderModule compModule = gpu::ShaderCompiler::CompileAndCreateModule(dev, compShaderGlsl, shaderc_glsl_compute_shader, "ComputeShader", &err);
    if (!compModule) {
        spdlog::error("ComputePass shader compilation failed: {}", err);
        return false;
    }

    // Descriptor set layout: binding 0 = storage image
    VkDescriptorSetLayoutBinding binding = {};
    binding.binding = 0;
    binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    binding.descriptorCount = 1;
    binding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &binding;

    if (vkCreateDescriptorSetLayout(dev, &layoutInfo, nullptr, &m_descriptorSetLayout) != VK_SUCCESS) {
        spdlog::error("Failed to create compute descriptor set layout");
        vkDestroyShaderModule(dev, compModule, nullptr);
        return false;
    }

    VkDescriptorPoolSize poolSize = {};
    poolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    poolSize.descriptorCount = 1;

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = 1;

    if (vkCreateDescriptorPool(dev, &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS) {
        spdlog::error("Failed to create compute descriptor pool");
        vkDestroyShaderModule(dev, compModule, nullptr);
        return false;
    }

    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &m_descriptorSetLayout;

    if (vkAllocateDescriptorSets(dev, &allocInfo, &m_descriptorSet) != VK_SUCCESS) {
        spdlog::error("Failed to allocate compute descriptor set");
        vkDestroyShaderModule(dev, compModule, nullptr);
        return false;
    }

    VkPushConstantRange pcRange = {};
    pcRange.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    pcRange.offset = 0;
    pcRange.size = static_cast<uint32_t>(pushConstantSize);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &m_descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = (pushConstantSize > 0) ? 1 : 0;
    pipelineLayoutInfo.pPushConstantRanges = (pushConstantSize > 0) ? &pcRange : nullptr;

    if (vkCreatePipelineLayout(dev, &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
        spdlog::error("Failed to create compute pipeline layout");
        vkDestroyShaderModule(dev, compModule, nullptr);
        return false;
    }

    VkComputePipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipelineInfo.layout = m_pipelineLayout;
    pipelineInfo.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    pipelineInfo.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    pipelineInfo.stage.module = compModule;
    pipelineInfo.stage.pName = "main";

    if (vkCreateComputePipelines(dev, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline) != VK_SUCCESS) {
        spdlog::error("Failed to create compute pipeline");
        vkDestroyShaderModule(dev, compModule, nullptr);
        return false;
    }

    vkDestroyShaderModule(dev, compModule, nullptr);
    return true;
}

void ComputePass::Dispatch(VkCommandBuffer cmd, gpu::Texture2D* outputImage, uint32_t groupCountX, uint32_t groupCountY, const void* pushConstantData, size_t pushConstantSize) {
    if (!outputImage || !m_pipeline) return;
    VkDevice dev = m_device->GetDevice();

    // 1. Transition output image to GENERAL layout for storage write
    outputImage->TransitionLayout(cmd, VK_IMAGE_LAYOUT_GENERAL,
                                  VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                                  VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_SHADER_WRITE_BIT);

    // 2. Update descriptor set with output storage image
    VkDescriptorImageInfo imageInfo = {};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    imageInfo.imageView = outputImage->GetImageView();

    VkWriteDescriptorSet write = {};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = m_descriptorSet;
    write.dstBinding = 0;
    write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    write.descriptorCount = 1;
    write.pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(dev, 1, &write, 0, nullptr);

    // 3. Bind pipeline & dispatch
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, m_pipeline);
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, m_pipelineLayout, 0, 1, &m_descriptorSet, 0, nullptr);

    if (pushConstantData && pushConstantSize > 0) {
        vkCmdPushConstants(cmd, m_pipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, static_cast<uint32_t>(pushConstantSize), pushConstantData);
    }

    vkCmdDispatch(cmd, groupCountX, groupCountY, 1);

    // 4. Memory barrier and transition back to SHADER_READ_ONLY_OPTIMAL
    outputImage->TransitionLayout(cmd, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                  VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                  VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT);
}

} // namespace render
