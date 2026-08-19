#pragma once

#include "EditorContext.h"

namespace nf::ui {

class MainMenuBar {
public:
    explicit MainMenuBar(EditorContext* ctx);
    void Render();

private:
    EditorContext* m_ctx;
    bool m_showAboutDialog = false;
};

} // namespace nf::ui
