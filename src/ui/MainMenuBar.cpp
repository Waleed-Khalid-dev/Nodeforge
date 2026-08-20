#include "MainMenuBar.h"
#include "commands/NodeCommands.h"
#include "../operators/comp/ContainerComp.h"
#include "../project/RecentProjectsManager.h"
#include <imgui.h>
#include <cstring>

namespace nf::ui {

MainMenuBar::MainMenuBar(EditorContext* ctx)
    : m_ctx(ctx) {
    if (m_ctx) {
        std::string recPath;
        if (m_ctx->GetAutosaveManager().HasNewerAutosave(m_ctx->GetProject().filePath, recPath)) {
            m_recoveryAutosavePath = recPath;
            m_showRecoveryDialog = true;
        }
    }
}

void MainMenuBar::Render() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Project", "Ctrl+N")) {
                m_ctx->NewProject();
            }
            if (ImGui::MenuItem("Open Project...", "Ctrl+O")) {
                m_showOpenDialog = true;
                m_statusMessage = "";
            }
            if (ImGui::MenuItem("Save Project", "Ctrl+S")) {
                if (m_ctx->GetProject().filePath.empty()) {
                    m_showSaveAsDialog = true;
                    m_statusMessage = "";
                } else {
                    std::string err;
                    if (!m_ctx->SaveProject("", &err)) {
                        m_statusMessage = err;
                    }
                }
            }
            if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) {
                m_showSaveAsDialog = true;
                m_statusMessage = "";
            }
            ImGui::Separator();

            auto* activeComp = dynamic_cast<ContainerComp*>(m_ctx->GetActiveNode());
            if (ImGui::MenuItem("Export Component...", nullptr, false, activeComp != nullptr)) {
                m_showExportCompDialog = true;
                m_statusMessage = "";
            }
            if (ImGui::MenuItem("Import Component...")) {
                m_showImportCompDialog = true;
                m_statusMessage = "";
            }

            ImGui::Separator();

            if (ImGui::BeginMenu("Recent Projects")) {
                const auto& recents = project::RecentProjectsManager::Instance().GetRecentProjects();
                if (recents.empty()) {
                    ImGui::TextDisabled("No recent files");
                } else {
                    for (const auto& path : recents) {
                        if (ImGui::MenuItem(path.c_str())) {
                            std::string err;
                            m_ctx->OpenProject(path, &err);
                        }
                    }
                    ImGui::Separator();
                    if (ImGui::MenuItem("Clear Recent List")) {
                        project::RecentProjectsManager::Instance().Clear();
                    }
                }
                ImGui::EndMenu();
            }

            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "Alt+F4")) {
                // Request exit
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Undo", "Ctrl+Z", false, m_ctx->GetUndoManager().CanUndo())) {
                m_ctx->GetUndoManager().Undo();
            }
            if (ImGui::MenuItem("Redo", "Ctrl+Y", false, m_ctx->GetUndoManager().CanRedo())) {
                m_ctx->GetUndoManager().Redo();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Delete Selected", "Del", false, !m_ctx->GetSelectedNodes().empty())) {
                for (NodeId selId : m_ctx->GetSelectedNodes()) {
                    m_ctx->GetUndoManager().ExecuteCommand(std::make_unique<DeleteNodeCommand>(m_ctx, selId));
                }
            }
            if (ImGui::MenuItem("Select All", "Ctrl+A")) {
                if (m_ctx->GetGraph()) {
                    for (const auto& [nodeId, node] : m_ctx->GetGraph()->GetNodes()) {
                        m_ctx->SelectNode(nodeId, true);
                    }
                }
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("Frame All", "F")) {
                // Canvas hotkey
            }
            if (ImGui::MenuItem("Reset Zoom", "Ctrl+0")) {
                m_ctx->GetCanvasZoom() = 1.0f;
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Operators")) {
            if (ImGui::MenuItem("Open OP Palette...", "TAB")) {
                m_ctx->OpenOpPalette(glm::vec2(ImGui::GetIO().DisplaySize.x * 0.4f, ImGui::GetIO().DisplaySize.y * 0.4f));
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Diagnostics")) {
            bool profilerOpen = m_ctx->IsProfilerOpen();
            if (ImGui::MenuItem("Profiler & Diagnostics", "Ctrl+P", &profilerOpen)) {
                m_ctx->SetProfilerOpen(profilerOpen);
            }
            bool hudOpen = m_ctx->IsPerformanceHUDOpen();
            if (ImGui::MenuItem("Performance HUD", "F3", &hudOpen)) {
                m_ctx->SetPerformanceHUDOpen(hudOpen);
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Plugins")) {
            bool pmOpen = m_ctx->IsPluginManagerOpen();
            if (ImGui::MenuItem("Plugin Manager...", "Ctrl+Shift+P", &pmOpen)) {
                m_ctx->SetPluginManagerOpen(pmOpen);
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About NodeForge")) {
                m_showAboutDialog = true;
            }
            ImGui::EndMenu();
        }

        // Project Status in Header
        std::string projName = m_ctx->GetProject().metadata.name;
        if (m_ctx->IsDirty()) projName += " *";
        ImVec2 titleSize = ImGui::CalcTextSize(projName.c_str());
        ImGui::SameLine(ImGui::GetWindowWidth() - titleSize.x - 20.0f);
        ImGui::TextColored(m_ctx->IsDirty() ? ImVec4(1.0f, 0.7f, 0.2f, 1.0f) : ImVec4(0.6f, 0.7f, 0.8f, 1.0f), "%s", projName.c_str());

        ImGui::EndMainMenuBar();
    }

    RenderFileModals();
    RenderCrashRecoveryModal();

    if (m_showAboutDialog) {
        ImGui::OpenPopup("About NodeForge");
        if (ImGui::BeginPopupModal("About NodeForge", &m_showAboutDialog, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::TextColored(ImVec4(0.95f, 0.60f, 0.15f, 1.0f), "NodeForge Studio");
            ImGui::Text("Version 0.1.0 (Phase 6 - Project System & Components)");
            ImGui::Text("Clean-Room Visual Node Engine for Live Visuals & Projection Mapping");
            ImGui::Text("Created for Neo Realms / Waleed Khalid");
            ImGui::Separator();
            ImGui::TextDisabled("Vulkan 1.3 | C++23 | ImGui | Python 3.11 | .nfp & .nfc");
            ImGui::Spacing();
            if (ImGui::Button("Close", ImVec2(120.0f, 0.0f))) {
                m_showAboutDialog = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
}

void MainMenuBar::RenderFileModals() {
    // Open Project Modal
    if (m_showOpenDialog) {
        ImGui::OpenPopup("Open NodeForge Project");
        m_showOpenDialog = false;
    }
    if (ImGui::BeginPopupModal("Open NodeForge Project", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Enter Project Filepath (.nfp):");
        ImGui::InputText("##OpenPath", m_pathBuffer, sizeof(m_pathBuffer));
        if (!m_statusMessage.empty()) {
            ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "%s", m_statusMessage.c_str());
        }
        ImGui::Spacing();
        if (ImGui::Button("Open", ImVec2(100.0f, 0.0f))) {
            std::string err;
            if (m_ctx->OpenProject(m_pathBuffer, &err)) {
                ImGui::CloseCurrentPopup();
            } else {
                m_statusMessage = err;
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(100.0f, 0.0f))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    // Save As Project Modal
    if (m_showSaveAsDialog) {
        ImGui::OpenPopup("Save NodeForge Project As");
        m_showSaveAsDialog = false;
    }
    if (ImGui::BeginPopupModal("Save NodeForge Project As", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Enter Destination Filepath (.nfp):");
        ImGui::InputText("##SaveAsPath", m_pathBuffer, sizeof(m_pathBuffer));
        if (!m_statusMessage.empty()) {
            ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "%s", m_statusMessage.c_str());
        }
        ImGui::Spacing();
        if (ImGui::Button("Save", ImVec2(100.0f, 0.0f))) {
            std::string err;
            if (m_ctx->SaveProject(m_pathBuffer, &err)) {
                ImGui::CloseCurrentPopup();
            } else {
                m_statusMessage = err;
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(100.0f, 0.0f))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    // Export Component Modal
    if (m_showExportCompDialog) {
        ImGui::OpenPopup("Export Component");
        m_showExportCompDialog = false;
    }
    if (ImGui::BeginPopupModal("Export Component", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Enter Component Filepath (.nfc):");
        ImGui::InputText("##ExportCompPath", m_compPathBuffer, sizeof(m_compPathBuffer));
        if (!m_statusMessage.empty()) {
            ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "%s", m_statusMessage.c_str());
        }
        ImGui::Spacing();
        if (ImGui::Button("Export", ImVec2(100.0f, 0.0f))) {
            auto* comp = dynamic_cast<ContainerComp*>(m_ctx->GetActiveNode());
            std::string err;
            if (m_ctx->ExportComponent(m_compPathBuffer, comp, &err)) {
                ImGui::CloseCurrentPopup();
            } else {
                m_statusMessage = err;
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(100.0f, 0.0f))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    // Import Component Modal
    if (m_showImportCompDialog) {
        ImGui::OpenPopup("Import Component");
        m_showImportCompDialog = false;
    }
    if (ImGui::BeginPopupModal("Import Component", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Enter Component Filepath (.nfc):");
        ImGui::InputText("##ImportCompPath", m_compPathBuffer, sizeof(m_compPathBuffer));
        if (!m_statusMessage.empty()) {
            ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "%s", m_statusMessage.c_str());
        }
        ImGui::Spacing();
        if (ImGui::Button("Import", ImVec2(100.0f, 0.0f))) {
            std::string err;
            if (m_ctx->ImportComponent(m_compPathBuffer, glm::vec2(150.0f, 150.0f), &err)) {
                ImGui::CloseCurrentPopup();
            } else {
                m_statusMessage = err;
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(100.0f, 0.0f))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void MainMenuBar::RenderCrashRecoveryModal() {
    if (m_showRecoveryDialog) {
        ImGui::OpenPopup("Crash Recovery Detected");
        m_showRecoveryDialog = false;
    }

    if (ImGui::BeginPopupModal("Crash Recovery Detected", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::TextColored(ImVec4(1.0f, 0.75f, 0.2f, 1.0f), "An unsaved project autosave was found from a previous session:");
        ImGui::TextDisabled("%s", m_recoveryAutosavePath.c_str());
        ImGui::Spacing();
        ImGui::Text("Would you like to recover your unsaved modifications?");
        ImGui::Spacing();

        if (ImGui::Button("Recover Project", ImVec2(140.0f, 0.0f))) {
            std::string err;
            m_ctx->OpenProject(m_recoveryAutosavePath, &err);
            m_ctx->MarkDirty(true);
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Discard Backup", ImVec2(140.0f, 0.0f))) {
            m_ctx->GetAutosaveManager().DiscardAutosave(m_ctx->GetProject().filePath);
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

} // namespace nf::ui
