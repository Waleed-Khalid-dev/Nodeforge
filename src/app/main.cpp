// NodeForge — Phase 0 shell
// Goal: GLFW window opens, runs, closes cleanly. Exit 0.
// No Vulkan, no operators, no graph — Phase 1 adds the GPU layer.
//
// Phase 0 Definition of Done:
//   cmake --build succeeds on clean Windows machine
//   Window opens and closes cleanly
//   No operator code
//
// Owner: Waleed Khalid | Neo Realms
// Roadmap: docs/02-BUILD-ROADMAP-A-to-Z.md

#define GLFW_INCLUDE_NONE   // we manage Vulkan headers ourselves
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include <fmt/core.h>

static constexpr int  NF_DEFAULT_WIDTH  = 1280;
static constexpr int  NF_DEFAULT_HEIGHT = 720;
static constexpr auto NF_WINDOW_TITLE   = "NodeForge v0.1 — Phase 0";

int main()
{
    spdlog::set_pattern("[%H:%M:%S] [%^%l%$] %v");
    spdlog::info("NodeForge {} — starting", NF_WINDOW_TITLE);

    if (!glfwInit())
    {
        spdlog::error("GLFW init failed");
        return 1;
    }

    // Phase 0: no GPU context — just a window handle
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE,  GLFW_TRUE);

    GLFWwindow* window = glfwCreateWindow(
        NF_DEFAULT_WIDTH, NF_DEFAULT_HEIGHT, NF_WINDOW_TITLE, nullptr, nullptr);

    if (!window)
    {
        spdlog::error("GLFW window creation failed");
        glfwTerminate();
        return 1;
    }

    spdlog::info("Window created ({}x{}). Press ESC or close to exit.",
                 NF_DEFAULT_WIDTH, NF_DEFAULT_HEIGHT);

    // Key callback: ESC closes
    glfwSetKeyCallback(window, [](GLFWwindow* w, int key, int, int action, int)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(w, GLFW_TRUE);
    });

    // ── Main loop ────────────────────────────────────────────────────────────
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        // Phase 1 will: submit GPU commands and present here
    }

    // ── Cleanup ──────────────────────────────────────────────────────────────
    spdlog::info("Shutting down cleanly.");
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
