#include "FullscreenPass.h"
#include <spdlog/spdlog.h>
#include <array>

namespace render {

FullscreenPass::FullscreenPass(gpu::Device* device) : m_device(device) {
}

FullscreenPass::~FullscreenPass() {
    Cleanup();
}

void FullscreenPass::Cleanup() {
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

bool FullscreenPass::Initialize(const std::string& fragShaderGlsl, uint32_t numSamplers, size_t pushConstantSize, VkFormat targetFormat) {
    Cleanup();
    m_numSamplers = numSamplers;
    VkDevice dev = m_device->GetDevice();

    // 1. Compile Shaders
    std::string err;
    VkShaderModule vertModule = gpu::ShaderCompiler::CompileAndCreateModule(dev, gpu::ShaderCompiler::GetFullscreenTriangleVertexShader(), shaderc_glsl_vertex_shader, "FullscreenVert", &err);
    if (!vertModule) {
        spdlog::error("FullscreenPass vertex shader compilation failed");
        return false;
    }

    VkShaderModule fragModule = gpu::ShaderCompiler::CompileAndCreateModule(dev, fragShaderGlsl, shaderc_glsl_fragment_shader, "FullscreenFrag", &err);
    if (!fragModule) {
        spdlog::error("FullscreenPass fragment shader compilation failed: {}", err);
        vkDestroyShaderModule(dev, vertModule, nullptr);
        return false;
    }

    // 2. Descriptor Layout & Pool
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    for (uint32_t i = 0; i < numSamplers; ++i) {
        VkDescriptorSetLayoutBinding b = {};
        b.binding = i;
        b.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        b.descriptorCount = 1;
        b.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        bindings.push_back(b);
    }

    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(dev, &layoutInfo, nullptr, &m_descriptorSetLayout) != VK_SUCCESS) {
        spdlog::error("Failed to create descriptor set layout");
        vkDestroyShaderModule(dev, vertModule, nullptr);
        vkDestroyShaderModule(dev, fragModule, nullptr);
        return false;
    }

    if (numSamplers > 0) {
        VkDescriptorPoolSize poolSize = {};
        poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSize.descriptorCount = numSamplers;

        VkDescriptorPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes = &poolSize;
        poolInfo.maxSets = 1;

        if (vkCreateDescriptorPool(dev, &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS) {
            spdlog::error("Failed to create descriptor pool");
            vkDestroyShaderModule(dev, vertModule, nullptr);
            vkDestroyShaderModule(dev, fragModule, nullptr);
            return false;
        }

        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = m_descriptorPool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &m_descriptorSetLayout;

        if (vkAllocateDescriptorSets(dev, &allocInfo, &m_descriptorSet) != VK_SUCCESS) {
            spdlog::error("Failed to allocate descriptor set");
            vkDestroyShaderModule(dev, vertModule, nullptr);
            vkDestroyShaderModule(dev, fragModule, nullptr);
            return false;
        }
    }

    // 3. Pipeline Layout
    VkPushConstantRange pcRange = {};
    pcRange.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    pcRange.offset = 0;
    pcRange.size = static_cast<uint32_t>(pushConstantSize);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &m_descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = (pushConstantSize > 0) ? 1 : 0;
    pipelineLayoutInfo.pPushConstantRanges = (pushConstantSize > 0) ? &pcRange : nullptr;

    if (vkCreatePipelineLayout(dev, &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
        spdlog::error("Failed to create pipeline layout");
        vkDestroyShaderModule(dev, vertModule, nullptr);
        vkDestroyShaderModule(dev, fragModule, nullptr);
        return false;
    }

    // 4. Graphics Pipeline with Dynamic Rendering
    VkPipelineShaderStageCreateInfo vertStageInfo = {};
    vertStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertStageInfo.module = vertModule;
    vertStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragStageInfo = {};
    fragStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragStageInfo.module = fragModule;
    fragStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertStageInfo, fragStageInfo };

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_NONE;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    std::array<VkDynamicState, 2> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    VkPipelineDynamicStateCreateInfo dynamicState = {};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    // Vulkan 1.3 Dynamic Rendering pipeline create info
    VkPipelineRenderingCreateInfo renderingCreateInfo = {};
    renderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    renderingCreateInfo.colorAttachmentCount = 1;
    renderingCreateInfo.pColorAttachmentFormats = &targetFormat;

    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.pNext = &renderingCreateInfo;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = m_pipelineLayout;

    if (vkCreateGraphicsPipelines(dev, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline) != VK_SUCCESS) {
        spdlog::error("Failed to create graphics pipeline");
        vkDestroyShaderModule(dev, vertModule, nullptr);
        vkDestroyShaderModule(dev, fragModule, nullptr);
        return false;
    }

    vkDestroyShaderModule(dev, vertModule, nullptr);
    vkDestroyShaderModule(dev, fragModule, nullptr);
    return true;
}

void FullscreenPass::Record(VkCommandBuffer cmd, gpu::Texture2D* target, const std::vector<gpu::Texture2D*>& inputSamplers, const void* pushConstantData, size_t pushConstantSize) {
    if (!target || !m_pipeline) return;
    VkDevice dev = m_device->GetDevice();

    // 1. Update Descriptor Sets if samplers are present
    if (m_descriptorSet && !inputSamplers.empty()) {
        std::vector<VkDescriptorImageInfo> imageInfos;
        std::vector<VkWriteDescriptorSet> writes;
        imageInfos.reserve(inputSamplers.size());
        writes.reserve(inputSamplers.size());

        for (size_t i = 0; i < inputSamplers.size(); ++i) {
            if (!inputSamplers[i]) continue;
            VkDescriptorImageInfo imgInfo = {};
            imgInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imgInfo.imageView = inputSamplers[i]->GetImageView();
            imgInfo.sampler = inputSamplers[i]->GetSampler();
            imageInfos.push_back(imgInfo);

            VkWriteDescriptorSet write = {};
            write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write.dstSet = m_descriptorSet;
            write.dstBinding = static_cast<uint32_t>(i);
            write.dstArrayElement = 0;
            write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            write.descriptorCount = 1;
            write.pImageInfo = &imageInfos.back();
            writes.push_back(write);
        }

        if (!writes.empty()) {
            vkUpdateDescriptorSets(dev, static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
        }
    }

    // 2. Transition target to COLOR_ATTACHMENT_OPTIMAL
    target->TransitionLayout(cmd, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                             VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                             VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);

    // 3. Begin Dynamic Rendering
    VkRenderingAttachmentInfo colorAttachment = {};
    colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    colorAttachment.imageView = target->GetImageView();
    colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    VkRenderingInfo renderingInfo = {};
    renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    renderingInfo.renderArea.offset = {0, 0};
    renderingInfo.renderArea.extent = { target->GetWidth(), target->GetHeight() };
    renderingInfo.layerCount = 1;
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachments = &colorAttachment;

    vkCmdBeginRendering(cmd, &renderingInfo);

    VkViewport viewport = { 0.0f, 0.0f, static_cast<float>(target->GetWidth()), static_cast<float>(target->GetHeight()), 0.0f, 1.0f };
    VkRect2D scissor = { {0, 0}, { target->GetWidth(), target->GetHeight() } };
    vkCmdSetViewport(cmd, 0, 1, &viewport);
    vkCmdSetScissor(cmd, 0, 1, &scissor);

    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
    if (m_descriptorSet && m_numSamplers > 0) {
        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1, &m_descriptorSet, 0, nullptr);
    }
    if (pushConstantData && pushConstantSize > 0) {
        vkCmdPushConstants(cmd, m_pipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, static_cast<uint32_t>(pushConstantSize), pushConstantData);
    }

    // Draw 3 vertices for full-screen triangle
    vkCmdDraw(cmd, 3, 1, 0, 0);

    vkCmdEndRendering(cmd);

    // 4. Transition target to SHADER_READ_ONLY_OPTIMAL for downstream nodes
    target->TransitionLayout(cmd, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                             VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                             VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT);
}

} // namespace render
