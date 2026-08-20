#pragma once

#include <string>
#include <memory>
#include <vector>
#include "../gpu/Device.h"
#include "../gpu/Swapchain.h"
#include "../gpu/FrameResources.h"
#include "../graph/Graph.h"
#include "../graph/CookContext.h"
#include "../project/ProjectFile.h"

struct GLFWwindow;

namespace nf::player {

struct PlayerOptions {
    std::string projectPath = "";
    bool fullscreen = false;
    bool borderless = false;
    bool kioskMode = false;
    int displayIndex = 0;
    int windowWidth = 1920;
    int windowHeight = 1080;
    double targetFps = 60.0;
    bool benchmarkMode = false;
    uint64_t maxFrames = 0;
};

class PlayerApp {
public:
    PlayerApp();
    ~PlayerApp();

    static PlayerOptions ParseArgs(int argc, char* argv[]);
    static void PrintHelp();

    bool Initialize(const PlayerOptions& options);
    int Run();
    void Shutdown();

    Graph& GetGraph() { return m_graph; }
    const PlayerOptions& GetOptions() const { return m_options; }

private:
    PlayerOptions m_options;
    GLFWwindow* m_window = nullptr;
    std::unique_ptr<::gpu::Device> m_device;
    std::unique_ptr<::gpu::Swapchain> m_swapchain;
    std::unique_ptr<::gpu::FrameResources> m_frames;

    Graph m_graph;
    CookContext m_cookContext;
    project::ProjectData m_projectData;
    bool m_isRunning = false;
};

} // namespace nf::player
