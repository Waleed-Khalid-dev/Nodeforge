#include "MainMenuBar.h"
#include "commands/NodeCommands.h"
#include <imgui.h>

namespace nf::ui {

MainMenuBar::MainMenuBar(EditorContext* ctx)
    : m_ctx(ctx) {
}

void MainMenuBar::Render() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Project", "Ctrl+N")) {
                if (m_ctx->GetGraph()) {
                    m_ctx->ClearSelection();
                    m_ctx->GetGraph()->Clear();
                }
            }
            if (ImGui::MenuItem("Open Project...", "Ctrl+O")) {
                // Open dialog
            }
            if (ImGui::MenuItem("Save Project", "Ctrl+S")) {
                // Save
            }
            if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) {
                // Save as
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
                // Handled in canvas
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

        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About NodeForge")) {
                m_showAboutDialog = true;
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    if (m_showAboutDialog) {
        ImGui::OpenPopup("About NodeForge");
        if (ImGui::BeginPopupModal("About NodeForge", &m_showAboutDialog, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::TextColored(ImVec4(0.95f, 0.60f, 0.15f, 1.0f), "NodeForge Studio");
            ImGui::Text("Version 0.1.0 (Phase 5 - Editor UI)");
            ImGui::Text("Clean-Room Visual Node Engine for Live Visuals & Projection Mapping");
            ImGui::Text("Created for Neo Realms / Waleed Khalid");
            ImGui::Separator();
            ImGui::TextDisabled("Vulkan 1.3 | C++23 | ImGui Docking | Python 3.11");
            ImGui::Spacing();
            if (ImGui::Button("Close", ImVec2(120.0f, 0.0f))) {
                m_showAboutDialog = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
}

} // namespace nf::ui
