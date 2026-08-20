#pragma once

#include "OutputWindow.h"
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

namespace nf {

struct DisplayInfo {
    int id = 0;
    std::string name;
    int x = 0;
    int y = 0;
    int width = 1920;
    int height = 1080;
    int refreshRate = 60;
    bool isPrimary = false;
};

class DisplayManager {
public:
    static DisplayManager& Instance();

    std::vector<DisplayInfo> EnumerateDisplays();
    OutputWindow* GetOrCreateWindow(int displayIndex, const std::string& title = "NodeForge Projector", bool fullscreen = true);
    void CloseWindow(int displayIndex);
    void CloseAllWindows();
    void PresentAll();

    size_t GetActiveWindowCount() const { return m_windows.size(); }
    bool HasWindow(int displayIndex) const { return m_windows.find(displayIndex) != m_windows.end(); }

private:
    DisplayManager() = default;
    ~DisplayManager() = default;

    std::unordered_map<int, std::unique_ptr<OutputWindow>> m_windows;
};

} // namespace nf
