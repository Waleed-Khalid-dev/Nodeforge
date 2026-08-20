#include "PlayerApp.h"
#include "../project/ProjectSerializer.h"
#include "../plugin/PluginManager.h"
#include "../diagnostics/CrashReporter.h"
#include "../media/DisplayManager.h"
#include <spdlog/spdlog.h>
#include <fmt/format.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <thread>
#include <chrono>
#include <iostream>

namespace nf::player {

PlayerOptions PlayerApp::ParseArgs(int argc, char* argv[]) {
    PlayerOptions opts;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if ((arg == "-p" || arg == "--project") && i + 1 < argc) {
            opts.projectPath = argv[++i];
        } else if (arg == "-f" || arg == "--fullscreen") {
            opts.fullscreen = true;
        } else if (arg == "--borderless") {
            opts.borderless = true;
        } else if (arg == "-k" || arg == "--kiosk") {
            opts.kioskMode = true;
            opts.fullscreen = true;
        } else if ((arg == "-d" || arg == "--display") && i + 1 < argc) {
            opts.displayIndex = std::stoi(argv[++i]);
        } else if (arg == "--fps" && i + 1 < argc) {
            opts.targetFps = std::stod(argv[++i]);
        } else if (arg == "--width" && i + 1 < argc) {
            opts.windowWidth = std::stoi(argv[++i]);
        } else if (arg == "--height" && i + 1 < argc) {
            opts.windowHeight = std::stoi(argv[++i]);
        } else if (arg == "--frames" && i + 1 < argc) {
            opts.maxFrames = std::stoull(argv[++i]);
        } else if (arg == "--benchmark") {
            opts.benchmarkMode = true;
        } else if (arg == "-h" || arg == "--help") {
            PrintHelp();
            std::exit(0);
        }
    }

    return opts;
}

void PlayerApp::PrintHelp() {
    std::cout << "NodeForge Player — Standalone Kiosk & Show Runtime\n"
              << "Usage: nodeforge_player [options]\n\n"
              << "Options:\n"
              << "  -p, --project <path>    Load and execute specified .nfp project file\n"
              << "  -f, --fullscreen        Start in exclusive fullscreen mode\n"
              << "      --borderless        Start in borderless windowed mode\n"
              << "  -k, --kiosk             Start in locked kiosk mode (fullscreen, hidden cursor)\n"
              << "  -d, --display <index>   Target display monitor index (default: 0)\n"
              << "      --fps <rate>        Target frame rate limit (default: 60.0)\n"
              << "      --width <pixels>    Window width for windowed mode (default: 1920)\n"
              << "      --height <pixels>   Window height for windowed mode (default: 1080)\n"
              << "      --frames <count>    Run N frames then exit (for automated testing/perf)\n"
              << "      --benchmark         Run as fast as possible without frame throttling\n"
              << "  -h, --help              Display this help message and exit\n";
}

PlayerApp::PlayerApp() = default;

PlayerApp::~PlayerApp() {
    Shutdown();
}

bool PlayerApp::Initialize(const PlayerOptions& options) {
    m_options = options;

    spdlog::info("Initializing NodeForge Player (Project: '{}')",
        m_options.projectPath.empty() ? "<Empty Graph>" : m_options.projectPath);

    // 1. Crash Reporter
    CrashReporter::Instance().Install();

    // 2. Discover Plugins
    PluginManager::Instance().ScanAndLoadPlugins();

    // 3. Load Project if provided
    if (!m_options.projectPath.empty()) {
        std::string err;
        if (!project::ProjectSerializer::LoadFromFile(m_options.projectPath, m_projectData, &err)) {
            spdlog::error("Failed to load project file '{}': {}", m_options.projectPath, err);
            return false;
        }
        if (m_projectData.rootGraph) {
            // Note: Transfer or use loaded graph
            spdlog::info("Successfully loaded project '{}' with metadata '{}'",
                m_options.projectPath, m_projectData.metadata.name);
        }
    }

    CrashReporter::Instance().SetActiveGraph(&m_graph);

    // 4. Initialize GLFW
    if (!glfwInit()) {
        spdlog::error("Failed to initialize GLFW");
        return false;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    GLFWmonitor* targetMonitor = nullptr;
    int monitorCount = 0;
    GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
    if (monitors && monitorCount > 0) {
        int idx = std::clamp(m_options.displayIndex, 0, monitorCount - 1);
        targetMonitor = monitors[idx];
    }

    if (m_options.fullscreen && targetMonitor) {
        const GLFWvidmode* mode = glfwGetVideoMode(targetMonitor);
        m_window = glfwCreateWindow(mode->width, mode->height, "NodeForge Player", targetMonitor, nullptr);
    } else {
        if (m_options.borderless) {
            glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
        }
        m_window = glfwCreateWindow(m_options.windowWidth, m_options.windowHeight, "NodeForge Player", nullptr, nullptr);
    }

    if (!m_window) {
        spdlog::error("Failed to create GLFW window");
        return false;
    }

    if (m_options.kioskMode) {
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }

    // 5. Vulkan Backend
    m_device = std::make_unique<::gpu::Device>();
    if (!m_device->Initialize(m_window)) {
        spdlog::error("Failed to initialize Vulkan device");
        return false;
    }
    m_swapchain = std::make_unique<::gpu::Swapchain>(m_device.get());
    if (!m_swapchain->Build()) {
        spdlog::error("Failed to build swapchain");
        return false;
    }
    m_frames = std::make_unique<::gpu::FrameResources>(m_device.get(), 2);
    if (!m_frames->Build()) {
        spdlog::error("Failed to build frame resources");
        return false;
    }

    m_cookContext.gpuDevice = m_device.get();
    m_cookContext.frameIndex = 1;
    m_cookContext.timeSeconds = 0.0;
    m_cookContext.deltaTimeSeconds = 1.0 / (m_options.targetFps > 0.0 ? m_options.targetFps : 60.0);

    m_isRunning = true;
    return true;
}

int PlayerApp::Run() {
    if (!m_isRunning || !m_window) return -1;

    spdlog::info("NodeForge Player running at target {} FPS...", m_options.targetFps);

    auto frameDuration = std::chrono::duration<double>(1.0 / (m_options.targetFps > 0.0 ? m_options.targetFps : 60.0));
    auto lastTime = std::chrono::high_resolution_clock::now();
    uint64_t frameCounter = 0;

    while (!glfwWindowShouldClose(m_window)) {
        auto frameStart = std::chrono::high_resolution_clock::now();
        glfwPollEvents();

        if (m_options.kioskMode) {
            // In kiosk mode, only Alt+F4 or termination signal can exit
        } else {
            if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                break;
            }
        }

        auto currentTime = std::chrono::high_resolution_clock::now();
        double dt = std::chrono::duration<double>(currentTime - lastTime).count();
        lastTime = currentTime;

        m_cookContext.frameIndex = ++frameCounter;
        m_cookContext.timeSeconds = static_cast<double>(frameCounter) / (m_options.targetFps > 0.0 ? m_options.targetFps : 60.0);
        m_cookContext.deltaTimeSeconds = dt;

        // Cook Graph
        m_graph.CookAll(m_cookContext);

        // Render Frame
        auto frame = m_frames->GetCurrentFrame();
        vkWaitForFences(m_device->GetDevice(), 1, &frame.inFlightFence, VK_TRUE, UINT64_MAX);

        uint32_t imageIndex = 0;
        VkResult res = vkAcquireNextImageKHR(m_device->GetDevice(), m_swapchain->GetSwapchain(), UINT64_MAX,
            frame.imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

        if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR) {
            vkDeviceWaitIdle(m_device->GetDevice());
            m_swapchain->Rebuild();
            continue;
        }

        vkResetFences(m_device->GetDevice(), 1, &frame.inFlightFence);
        vkResetCommandBuffer(frame.commandBuffer, 0);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        vkBeginCommandBuffer(frame.commandBuffer, &beginInfo);

        // Clear swapchain color attachment
        VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
        VkRenderingAttachmentInfoKHR colorAttachment{};
        colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
        colorAttachment.imageView = m_swapchain->GetImageViews()[imageIndex];
        colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.clearValue = clearColor;

        // Transition layout
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = m_swapchain->GetImages()[imageIndex];
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.layerCount = 1;

        vkCmdPipelineBarrier(frame.commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

        VkRenderingInfoKHR renderingInfo{};
        renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
        renderingInfo.renderArea.extent = m_swapchain->GetExtent();
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.pColorAttachments = &colorAttachment;

        vkCmdBeginRendering(frame.commandBuffer, &renderingInfo);
        vkCmdEndRendering(frame.commandBuffer);

        // Transition to Present
        barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        vkCmdPipelineBarrier(frame.commandBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

        vkEndCommandBuffer(frame.commandBuffer);

        VkSemaphore waitSemas[] = { frame.imageAvailableSemaphore };
        VkSemaphore signalSemas[] = { frame.renderFinishedSemaphore };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemas;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &frame.commandBuffer;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemas;

        vkQueueSubmit(m_device->GetGraphicsQueue(), 1, &submitInfo, frame.inFlightFence);

        VkSwapchainKHR swapchains[] = { m_swapchain->GetSwapchain() };
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemas;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapchains;
        presentInfo.pImageIndices = &imageIndex;

        vkQueuePresentKHR(m_device->GetPresentQueue(), &presentInfo);

        if (m_options.maxFrames > 0 && frameCounter >= m_options.maxFrames) {
            spdlog::info("Reached maximum frames limit ({})", m_options.maxFrames);
            break;
        }

        // Frame rate pacing
        if (!m_options.benchmarkMode) {
            auto frameEnd = std::chrono::high_resolution_clock::now();
            auto elapsed = frameEnd - frameStart;
            if (elapsed < frameDuration) {
                std::this_thread::sleep_for(frameDuration - elapsed);
            }
        }
    }

    if (m_device) {
        vkDeviceWaitIdle(m_device->GetDevice());
    }

    spdlog::info("NodeForge Player shutdown cleanly after {} frames", frameCounter);
    return 0;
}

void PlayerApp::Shutdown() {
    if (!m_isRunning) return;

    if (m_device) {
        vkDeviceWaitIdle(m_device->GetDevice());
    }

    m_frames.reset();
    m_swapchain.reset();
    m_device.reset();

    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }

    glfwTerminate();
    m_isRunning = false;
}

} // namespace nf::player
