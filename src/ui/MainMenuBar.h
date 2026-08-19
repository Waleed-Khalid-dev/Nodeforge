#pragma once

#include "EditorContext.h"
#include <string>

namespace nf::ui {

class MainMenuBar {
public:
    explicit MainMenuBar(EditorContext* ctx);
    void Render();

private:
    void RenderFileModals();
    void RenderCrashRecoveryModal();

    EditorContext* m_ctx;
    bool m_showAboutDialog = false;

    bool m_showOpenDialog = false;
    bool m_showSaveAsDialog = false;
    bool m_showExportCompDialog = false;
    bool m_showImportCompDialog = false;
    bool m_showRecoveryDialog = false;

    std::string m_recoveryAutosavePath = "";
    char m_pathBuffer[512] = "project.nfp";
    char m_compPathBuffer[512] = "component.nfc";
    std::string m_statusMessage = "";
};

} // namespace nf::ui
