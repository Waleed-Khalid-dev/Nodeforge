#pragma once

#include <string>
#include <memory>
#include <glm/glm.hpp>
#include "../gpu/Texture.h"

namespace nf {

class OutputWindow {
public:
    OutputWindow(int displayIndex, const std::string& title, int width, int height, bool fullscreen);
    ~OutputWindow();

    void SetTexture(std::shared_ptr<gpu::Texture2D> tex) { m_currentTexture = tex; }
    std::shared_ptr<gpu::Texture2D> GetTexture() const { return m_currentTexture; }

    void SetCalibrationMode(bool enabled) { m_calibrationMode = enabled; }
    bool IsCalibrationMode() const { return m_calibrationMode; }

    int GetDisplayIndex() const { return m_displayIndex; }
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    bool IsFullscreen() const { return m_fullscreen; }
    bool IsOpen() const { return m_isOpen; }
    void Close() { m_isOpen = false; }

    void Present();

private:
    int m_displayIndex = 0;
    std::string m_title;
    int m_width = 1920;
    int m_height = 1080;
    bool m_fullscreen = false;
    bool m_calibrationMode = false;
    bool m_isOpen = true;
    std::shared_ptr<gpu::Texture2D> m_currentTexture;
};

} // namespace nf
