#pragma once

#include "../EditorContext.h"

namespace nf::ui {

class TimelinePanel {
public:
    explicit TimelinePanel(EditorContext* ctx);
    void Render();

private:
    EditorContext* m_ctx;
    int m_startFrame = 1;
    int m_endFrame = 600;
};

} // namespace nf::ui
