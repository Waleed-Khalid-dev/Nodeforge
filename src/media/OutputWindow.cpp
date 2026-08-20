#include "OutputWindow.h"

namespace nf {

OutputWindow::OutputWindow(int displayIndex, const std::string& title, int width, int height, bool fullscreen)
    : m_displayIndex(displayIndex), m_title(title), m_width(width), m_height(height), m_fullscreen(fullscreen), m_isOpen(true) {
}

OutputWindow::~OutputWindow() {
    Close();
}

void OutputWindow::Present() {
    if (!m_isOpen) return;
    // Swapchain presentation hook
}

} // namespace nf
