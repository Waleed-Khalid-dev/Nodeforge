#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include <fmt/core.h>
#include <fstream>
#include <vector>
#include <iostream>

#include "../gpu/Device.h"
#include "../gpu/Swapchain.h"
#include "../gpu/Texture.h"
#include "../gpu/FrameResources.h"

#include <shaderc/shaderc.hpp>

static constexpr int  NF_DEFAULT_WIDTH  = 1280;
static constexpr int  NF_DEFAULT_HEIGHT = 720;
static constexpr auto NF_WINDOW_TITLE   = "NodeForge v0.1 — Phase 1";

std::vector<uint32_t> CompileShader(const std::string& source, shaderc_shader_kind kind, const std::string& name) {
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;
    options.SetOptimizationLevel(shaderc_optimization_level_performance);
    
    shaderc::SpvCompilationResult module =
        compiler.CompileGlslToSpv(source, kind, name.c_str(), options);

    if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
        spdlog::error("Shader compile error in {}: {}", name, module.GetErrorMessage());
        return {};
    }
    return {module.cbegin(), module.cend()};
}

std::string ReadFile(const std::string& filepath) {
    std::vector<std::string> searchPaths = {
        filepath,
        "../../" + filepath,
        "../" + filepath,
        "D:/[Project]/Touch Designer/" + filepath
    };
    for (const auto& path : searchPaths) {
        std::ifstream file(path, std::ios::ate | std::ios::binary);
        if (file.is_open()) {
            size_t fileSize = (size_t)file.tellg();
            std::string buffer;
            buffer.resize(fileSize);
            file.seekg(0);
            file.read(buffer.data(), fileSize);
            file.close();
            return buffer;
        }
    }
    spdlog::error("Failed to open file: {}", filepath);
    return "";
}

VkShaderModule CreateShaderModule(VkDevice device, const std::vector<uint32_t>& code) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size() * sizeof(uint32_t);
    createInfo.pCode = code.data();

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        spdlog::error("Failed to create shader module");
        return VK_NULL_HANDLE;
    }
    return shaderModule;
}

int main() {
    spdlog::set_pattern("[%H:%M:%S] [%^%l%$] %v");
    spdlog::info("NodeForge {} — starting", NF_WINDOW_TITLE);

    if (!glfwInit()) {
        spdlog::error("GLFW init failed");
        return 1;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    GLFWwindow* window = glfwCreateWindow(NF_DEFAULT_WIDTH, NF_DEFAULT_HEIGHT, NF_WINDOW_TITLE, nullptr, nullptr);
    if (!window) {
        spdlog::error("GLFW window creation failed");
        glfwTerminate();
        return 1;
    }

    gpu::Device device;
    if (!device.Initialize(window)) return -1;

    gpu::Swapchain swapchain(&device);
    if (!swapchain.Build()) return -1;

    gpu::Texture2D checkerboard(&device);
    if (!checkerboard.CreateProceduralCheckerboard()) return -1;

    gpu::FrameResources frames(&device, 2);
    if (!frames.Build()) return -1;

    // Read & Compile Shaders
    std::string vertSource = ReadFile("shaders/fullscreen.vert");
    std::string fragSource = ReadFile("shaders/texture.frag");
    
    auto vertSpv = CompileShader(vertSource, shaderc_vertex_shader, "fullscreen.vert");
    auto fragSpv = CompileShader(fragSource, shaderc_fragment_shader, "texture.frag");

    VkDevice vkDev = device.GetDevice();
    VkShaderModule vertModule = CreateShaderModule(vkDev, vertSpv);
    VkShaderModule fragModule = CreateShaderModule(vkDev, fragSpv);

    // Descriptor Set Layout
    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = 0;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &samplerLayoutBinding;

    VkDescriptorSetLayout descriptorSetLayout;
    vkCreateDescriptorSetLayout(vkDev, &layoutInfo, nullptr, &descriptorSetLayout);

    // Descriptor Pool & Set
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSize.descriptorCount = 1;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = 1;

    VkDescriptorPool descriptorPool;
    vkCreateDescriptorPool(vkDev, &poolInfo, nullptr, &descriptorPool);

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &descriptorSetLayout;

    VkDescriptorSet descriptorSet;
    vkAllocateDescriptorSets(vkDev, &allocInfo, &descriptorSet);

    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = checkerboard.GetImageView();
    imageInfo.sampler = checkerboard.GetSampler();

    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = descriptorSet;
    descriptorWrite.dstBinding = 0;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(vkDev, 1, &descriptorWrite, 0, nullptr);

    // Pipeline Layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
    VkPipelineLayout pipelineLayout;
    vkCreatePipelineLayout(vkDev, &pipelineLayoutInfo, nullptr, &pipelineLayout);

    // Graphics Pipeline (Dynamic Rendering)
    VkPipelineShaderStageCreateInfo vertStageInfo{};
    vertStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertStageInfo.module = vertModule;
    vertStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragStageInfo{};
    fragStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragStageInfo.module = fragModule;
    fragStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertStageInfo, fragStageInfo};

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    // Dynamic Rendering Info
    VkPipelineRenderingCreateInfo pipelineRenderingInfo{};
    pipelineRenderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    VkFormat colorFormat = swapchain.GetImageFormat();
    pipelineRenderingInfo.colorAttachmentCount = 1;
    pipelineRenderingInfo.pColorAttachmentFormats = &colorFormat;

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.pNext = &pipelineRenderingInfo;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = VK_NULL_HANDLE; // NO RENDERPASS needed!

    VkPipeline graphicsPipeline;
    vkCreateGraphicsPipelines(vkDev, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline);

    vkDestroyShaderModule(vkDev, fragModule, nullptr);
    vkDestroyShaderModule(vkDev, vertModule, nullptr);

    // Main Loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        auto frame = frames.GetCurrentFrame();
        vkWaitForFences(vkDev, 1, &frame.inFlightFence, VK_TRUE, UINT64_MAX);

        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(vkDev, swapchain.GetSwapchain(), UINT64_MAX, frame.imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
            int width = 0, height = 0;
            glfwGetFramebufferSize(window, &width, &height);
            while (width == 0 || height == 0) {
                glfwGetFramebufferSize(window, &width, &height);
                glfwWaitEvents();
            }
            vkDeviceWaitIdle(vkDev);
            swapchain.Rebuild();
            continue;
        }

        vkResetFences(vkDev, 1, &frame.inFlightFence);
        vkResetCommandBuffer(frame.commandBuffer, 0);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        vkBeginCommandBuffer(frame.commandBuffer, &beginInfo);

        // Transition Image for Color Attachment
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = swapchain.GetImages()[imageIndex];
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        vkCmdPipelineBarrier(frame.commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                             0, 0, nullptr, 0, nullptr, 1, &barrier);

        VkRenderingAttachmentInfo colorAttachment{};
        colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        colorAttachment.imageView = swapchain.GetImageViews()[imageIndex];
        colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.clearValue.color = {0.1f, 0.1f, 0.1f, 1.0f};

        VkRenderingInfo renderingInfo{};
        renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        renderingInfo.renderArea.offset = {0, 0};
        renderingInfo.renderArea.extent = swapchain.GetExtent();
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.pColorAttachments = &colorAttachment;

        vkCmdBeginRendering(frame.commandBuffer, &renderingInfo);

        vkCmdBindPipeline(frame.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)swapchain.GetExtent().width;
        viewport.height = (float)swapchain.GetExtent().height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(frame.commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = swapchain.GetExtent();
        vkCmdSetScissor(frame.commandBuffer, 0, 1, &scissor);

        vkCmdBindDescriptorSets(frame.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

        vkCmdDraw(frame.commandBuffer, 3, 1, 0, 0);

        vkCmdEndRendering(frame.commandBuffer);

        // Transition Image for Present
        barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        barrier.dstAccessMask = 0;

        vkCmdPipelineBarrier(frame.commandBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                             0, 0, nullptr, 0, nullptr, 1, &barrier);

        vkEndCommandBuffer(frame.commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        VkSemaphore waitSemaphores[] = {frame.imageAvailableSemaphore};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &frame.commandBuffer;
        VkSemaphore signalSemaphores[] = {frame.renderFinishedSemaphore};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        vkQueueSubmit(device.GetGraphicsQueue(), 1, &submitInfo, frame.inFlightFence);

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;
        VkSwapchainKHR swapchains[] = {swapchain.GetSwapchain()};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapchains;
        presentInfo.pImageIndices = &imageIndex;

        result = vkQueuePresentKHR(device.GetPresentQueue(), &presentInfo);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
            vkDeviceWaitIdle(vkDev);
            swapchain.Rebuild();
        }

        frames.AdvanceFrame();
    }

    vkDeviceWaitIdle(vkDev);

    vkDestroyPipeline(vkDev, graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(vkDev, pipelineLayout, nullptr);
    vkDestroyDescriptorPool(vkDev, descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(vkDev, descriptorSetLayout, nullptr);

    frames.Cleanup();
    checkerboard.Cleanup();
    swapchain.Cleanup();
    device.Cleanup();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
