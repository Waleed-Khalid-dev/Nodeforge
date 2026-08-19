#pragma once

#include "../EditorContext.h"
#include <imgui.h>

namespace nf::ui {

class BreadcrumbBar {
public:
    explicit BreadcrumbBar(EditorContext* ctx);
    ~BreadcrumbBar() = default;

    // Renders the breadcrumb address bar at the top of the canvas
    void Render();

private:
    EditorContext* m_ctx = nullptr;
};

} // namespace nf::ui
