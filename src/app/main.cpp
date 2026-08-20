#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include <fmt/core.h>
#include <iostream>
#include <chrono>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include "../gpu/Device.h"
#include "../gpu/Swapchain.h"
#include "../gpu/FrameResources.h"
#include "../graph/Graph.h"
#include "../graph/CoreNodes.h"
#include "../python/PythonEngine.h"

#include "../ui/EditorTheme.h"
#include "../ui/EditorContext.h"
#include "../ui/MainMenuBar.h"
#include "../ui/canvas/NodeCanvas.h"
#include "../ui/panels/OpPaletteModal.h"
#include "../ui/panels/ParameterPanel.h"
#include "../ui/panels/ViewerPanel.h"
#include "../ui/panels/ConsolePanel.h"
#include "../ui/panels/TimelinePanel.h"
#include "../ui/panels/ProfilerPanel.h"
#include "../ui/panels/PerformanceHUD.h"
#include "../profiling/CookProfiler.h"
#include "../gpu/GpuTimerPool.h"
#include "../diagnostics/CrashReporter.h"

static constexpr int  NF_DEFAULT_WIDTH  = 1600;
static constexpr int  NF_DEFAULT_HEIGHT = 900;
static constexpr auto NF_WINDOW_TITLE   = "NodeForge Studio — Visual Operator Environment";

int main() {
    spdlog::set_pattern("[%H:%M:%S] [%^%l%$] %v");
    spdlog::info("Starting {}", NF_WINDOW_TITLE);

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
    if (!device.Initialize(window)) {
        spdlog::error("Vulkan device initialization failed");
        return -1;
    }

    gpu::Swapchain swapchain(&device);
    if (!swapchain.Build()) {
        spdlog::error("Swapchain build failed");
        return -1;
    }

    gpu::FrameResources frames(&device, 2);
    if (!frames.Build()) {
        spdlog::error("Frame resources build failed");
        return -1;
    }

    // ─── Initialize Python Runtime & Core Operators ───────────────────────────
    nf::PythonEngine::Instance().Initialize();
    nf::RegisterCoreNodes(nf::NodeRegistry::Instance());

    // ─── Initialize Graph & Demo Pipeline ──────────────────────────────────────
    nf::Graph graph;
    nf::CookContext cookContext;
    cookContext.gpuDevice = &device;
    cookContext.frameIndex = 1;
    cookContext.timeSeconds = 0.0;
    cookContext.deltaTimeSeconds = 1.0 / 60.0;

    // Create starter nodes
    auto constNode = nf::NodeRegistry::Instance().CreateNode("ConstantTexOp", graph.GenerateNodeId(), "Constant1");
    auto blurNode = nf::NodeRegistry::Instance().CreateNode("BlurTexOp", graph.GenerateNodeId(), "Blur1");
    auto levelNode = nf::NodeRegistry::Instance().CreateNode("LevelTexOp", graph.GenerateNodeId(), "Level1");
    auto toWinNode = nf::NodeRegistry::Instance().CreateNode("ToWindowTexOp", graph.GenerateNodeId(), "ToWindow1");

    nf::NodeId constId = constNode->GetId();
    nf::NodeId blurId = blurNode->GetId();
    nf::NodeId levelId = levelNode->GetId();
    nf::NodeId toWinId = toWinNode->GetId();

    graph.AddNode(std::move(constNode));
    graph.AddNode(std::move(blurNode));
    graph.AddNode(std::move(levelNode));
    graph.AddNode(std::move(toWinNode));

    graph.Connect(graph.GetNode(constId)->GetOutputPin(0), graph.GetNode(blurId)->GetInputPin(0));
    graph.Connect(graph.GetNode(blurId)->GetOutputPin(0), graph.GetNode(levelId)->GetInputPin(0));
    graph.Connect(graph.GetNode(levelId)->GetOutputPin(0), graph.GetNode(toWinId)->GetInputPin(0));

    // ─── Setup ImGui Context & Vulkan Backend ────────────────────────────────
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    nf::ui::EditorTheme::ApplyTheme();

    ImGui_ImplGlfw_InitForVulkan(window, true);

    VkFormat colorFormat = swapchain.GetImageFormat();

    ImGui_ImplVulkan_InitInfo initInfo{};
    initInfo.ApiVersion = VK_API_VERSION_1_3;
    initInfo.Instance = device.GetInstance();
    initInfo.PhysicalDevice = device.GetPhysicalDevice();
    initInfo.Device = device.GetDevice();
    initInfo.QueueFamily = device.GetGraphicsQueueIndex();
    initInfo.Queue = device.GetGraphicsQueue();
    initInfo.DescriptorPoolSize = 100;
    initInfo.MinImageCount = 2;
    initInfo.ImageCount = static_cast<uint32_t>(swapchain.GetImages().size());
    initInfo.UseDynamicRendering = true;
    initInfo.PipelineInfoMain.PipelineRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
    initInfo.PipelineInfoMain.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
    initInfo.PipelineInfoMain.PipelineRenderingCreateInfo.pColorAttachmentFormats = &colorFormat;

    if (!ImGui_ImplVulkan_Init(&initInfo)) {
        spdlog::error("ImGui Vulkan backend initialization failed");
        return -1;
    }

    // ─── Setup UI & Profiling Subsystems ───────────────────────────────────────
    nf::CrashReporter::Instance().Install();
    nf::CrashReporter::Instance().SetActiveGraph(&graph);
    nf::GpuTimerPool::Instance().Initialize(&device);

    nf::ui::EditorContext editorCtx;
    editorCtx.SetGraph(&graph);
    editorCtx.SetNodePosition(constId, glm::vec2(100.0f, 150.0f));
    editorCtx.SetNodePosition(blurId, glm::vec2(360.0f, 150.0f));
    editorCtx.SetNodePosition(levelId, glm::vec2(620.0f, 150.0f));
    editorCtx.SetNodePosition(toWinId, glm::vec2(880.0f, 150.0f));
    editorCtx.SelectNode(constId);

    nf::ui::MainMenuBar mainMenuBar(&editorCtx);
    nf::ui::NodeCanvas nodeCanvas(&editorCtx);
    nf::ui::OpPaletteModal opPaletteModal(&editorCtx);
    nf::ui::ParameterPanel paramPanel(&editorCtx);
    nf::ui::ViewerPanel viewerPanel(&editorCtx);
    nf::ui::ConsolePanel consolePanel(&editorCtx);
    nf::ui::TimelinePanel timelinePanel(&editorCtx);
    nf::ui::ProfilerPanel profilerPanel(&editorCtx);
    nf::ui::PerformanceHUD performanceHud(&editorCtx);

    auto lastTime = std::chrono::high_resolution_clock::now();

    // ─── Main Render & Interactive Loop ────────────────────────────────────────
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        auto currentTime = std::chrono::high_resolution_clock::now();
        double dt = std::chrono::duration<double>(currentTime - lastTime).count();
        lastTime = currentTime;

        nf::CookProfiler::Instance().BeginFrame(cookContext.frameIndex, dt);

        // Cook Graph
        if (editorCtx.IsPlaying()) {
            cookContext.frameIndex = editorCtx.GetCurrentFrame();
            cookContext.timeSeconds = static_cast<double>(cookContext.frameIndex) / 60.0;
            cookContext.deltaTimeSeconds = dt;
            graph.CookAll(cookContext);
            editorCtx.SetCurrentFrame(cookContext.frameIndex + 1);
        }

        nf::GpuTimerPool::Instance().ResolveResults();

        // ImGui New Frame
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Keyboard Shortcuts
        if (ImGui::IsKeyPressed(ImGuiKey_F3, false)) {
            editorCtx.TogglePerformanceHUD();
        }
        if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_P, false)) {
            editorCtx.ToggleProfiler();
        }

        // Render UI Components
        mainMenuBar.Render();
        nodeCanvas.Render();
        paramPanel.Render();
        viewerPanel.Render();
        consolePanel.Render();
        timelinePanel.Render();
        opPaletteModal.Render();

        if (editorCtx.IsProfilerOpen()) {
            bool open = true;
            profilerPanel.Render(&open);
            if (!open) editorCtx.SetProfilerOpen(false);
        }

        if (editorCtx.IsPerformanceHUDOpen()) {
            bool open = true;
            performanceHud.Render(&open);
            if (!open) editorCtx.SetPerformanceHUDOpen(false);
        }

        nf::CookProfiler::Instance().EndFrame();

        ImGui::Render();

        // Vulkan Frame Rendering
        auto frame = frames.GetCurrentFrame();
        vkWaitForFences(device.GetDevice(), 1, &frame.inFlightFence, VK_TRUE, UINT64_MAX);

        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(device.GetDevice(), swapchain.GetSwapchain(), UINT64_MAX, frame.imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
            int width = 0, height = 0;
            glfwGetFramebufferSize(window, &width, &height);
            while (width == 0 || height == 0) {
                glfwGetFramebufferSize(window, &width, &height);
                glfwWaitEvents();
            }
            vkDeviceWaitIdle(device.GetDevice());
            swapchain.Rebuild();
            continue;
        }

        vkResetFences(device.GetDevice(), 1, &frame.inFlightFence);
        vkResetCommandBuffer(frame.commandBuffer, 0);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        vkBeginCommandBuffer(frame.commandBuffer, &beginInfo);

        // Transition Swapchain Image for Dynamic Rendering Color Attachment
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
        colorAttachment.clearValue.color = { 0.08f, 0.09f, 0.11f, 1.0f };

        VkRenderingInfo renderingInfo{};
        renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        renderingInfo.renderArea.offset = { 0, 0 };
        renderingInfo.renderArea.extent = swapchain.GetExtent();
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.pColorAttachments = &colorAttachment;

        vkCmdBeginRendering(frame.commandBuffer, &renderingInfo);

        // Draw ImGui
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), frame.commandBuffer);

        vkCmdEndRendering(frame.commandBuffer);

        // Transition Swapchain Image for Present
        barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        barrier.dstAccessMask = 0;

        vkCmdPipelineBarrier(frame.commandBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                             0, 0, nullptr, 0, nullptr, 1, &barrier);

        vkEndCommandBuffer(frame.commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        VkSemaphore waitSemaphores[] = { frame.imageAvailableSemaphore };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &frame.commandBuffer;
        VkSemaphore signalSemaphores[] = { frame.renderFinishedSemaphore };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        vkQueueSubmit(device.GetGraphicsQueue(), 1, &submitInfo, frame.inFlightFence);

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;
        VkSwapchainKHR swapchains[] = { swapchain.GetSwapchain() };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapchains;
        presentInfo.pImageIndices = &imageIndex;

        result = vkQueuePresentKHR(device.GetPresentQueue(), &presentInfo);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
            vkDeviceWaitIdle(device.GetDevice());
            swapchain.Rebuild();
        }

        frames.AdvanceFrame();
    }

    // ─── Cleanup ───────────────────────────────────────────────────────────────
    vkDeviceWaitIdle(device.GetDevice());

    graph.Clear();

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    frames.Cleanup();
    swapchain.Cleanup();
    device.Cleanup();

    nf::PythonEngine::Instance().Shutdown();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
