#include "DisplayManager.h"
#include <GLFW/glfw3.h>

namespace nf {

DisplayManager& DisplayManager::Instance() {
    static DisplayManager s_instance;
    return s_instance;
}

std::vector<DisplayInfo> DisplayManager::EnumerateDisplays() {
    std::vector<DisplayInfo> displays;

    int monitorCount = 0;
    GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);

    if (monitors && monitorCount > 0) {
        for (int i = 0; i < monitorCount; ++i) {
            GLFWmonitor* m = monitors[i];
            const char* name = glfwGetMonitorName(m);
            const GLFWvidmode* mode = glfwGetVideoMode(m);
            int xpos = 0, ypos = 0;
            glfwGetMonitorPos(m, &xpos, &ypos);

            DisplayInfo info{};
            info.id = i;
            info.name = name ? name : ("Display " + std::to_string(i));
            info.x = xpos;
            info.y = ypos;
            info.width = mode ? mode->width : 1920;
            info.height = mode ? mode->height : 1080;
            info.refreshRate = mode ? mode->refreshRate : 60;
            info.isPrimary = (i == 0);

            displays.push_back(info);
        }
    } else {
        // Fallback default displays for headless or simulated testing
        displays.push_back(DisplayInfo{ .id = 0, .name = "Primary Display (1080p)", .x = 0, .y = 0, .width = 1920, .height = 1080, .refreshRate = 60, .isPrimary = true });
        displays.push_back(DisplayInfo{ .id = 1, .name = "Projector 1 (Left Facade)", .x = 1920, .y = 0, .width = 1920, .height = 1080, .refreshRate = 60, .isPrimary = false });
        displays.push_back(DisplayInfo{ .id = 2, .name = "Projector 2 (Right Facade)", .x = 3840, .y = 0, .width = 1920, .height = 1080, .refreshRate = 60, .isPrimary = false });
    }

    return displays;
}

OutputWindow* DisplayManager::GetOrCreateWindow(int displayIndex, const std::string& title, bool fullscreen) {
    auto it = m_windows.find(displayIndex);
    if (it != m_windows.end()) {
        return it->second.get();
    }

    auto displays = EnumerateDisplays();
    int w = 1920, h = 1080;
    for (const auto& d : displays) {
        if (d.id == displayIndex) {
            w = d.width;
            h = d.height;
            break;
        }
    }

    auto win = std::make_unique<OutputWindow>(displayIndex, title, w, h, fullscreen);
    OutputWindow* ptr = win.get();
    m_windows[displayIndex] = std::move(win);
    return ptr;
}

void DisplayManager::CloseWindow(int displayIndex) {
    m_windows.erase(displayIndex);
}

void DisplayManager::CloseAllWindows() {
    m_windows.clear();
}

void DisplayManager::PresentAll() {
    for (auto& [id, win] : m_windows) {
        if (win) {
            win->Present();
        }
    }
}

} // namespace nf
