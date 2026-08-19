#include "OpPaletteModal.h"
#include "../EditorTheme.h"
#include "../commands/NodeCommands.h"
#include <imgui.h>
#include <algorithm>

namespace nf::ui {

OpPaletteModal::OpPaletteModal(EditorContext* ctx)
    : m_ctx(ctx) {
}

void OpPaletteModal::Render() {
    if (!m_ctx->IsOpPaletteOpen()) return;

    ImGui::OpenPopup("Create Operator");
    ImGui::SetNextWindowPos(ImVec2(m_ctx->GetOpPalettePos().x, m_ctx->GetOpPalettePos().y), ImGuiCond_Appearing);
    ImGui::SetNextWindowSize(ImVec2(380.0f, 420.0f), ImGuiCond_Appearing);

    if (ImGui::BeginPopupModal("Create Operator", nullptr, ImGuiWindowFlags_NoResize)) {
        // Search Input
        ImGui::SetNextItemWidth(-1.0f);
        if (ImGui::IsWindowAppearing()) {
            ImGui::SetKeyboardFocusHere();
            m_searchQuery[0] = '\0';
            m_highlightedItemIndex = 0;
        }
        ImGui::InputTextWithHint("##Search", "Search operators...", m_searchQuery, IM_ARRAYSIZE(m_searchQuery));

        ImGui::Spacing();

        // Family Tabs
        const char* families[] = { "ALL", "TexOp", "ChanOp", "GeomOp", "MatOp", "DataOp", "Comp" };
        if (ImGui::BeginTabBar("FamilyTabs")) {
            for (int i = 0; i < 7; ++i) {
                if (ImGui::BeginTabItem(families[i])) {
                    m_selectedFamilyIndex = i;
                    ImGui::EndTabItem();
                }
            }
            ImGui::EndTabBar();
        }

        ImGui::Spacing();

        // Filter and collect nodes
        std::string query = m_searchQuery;
        std::transform(query.begin(), query.end(), query.begin(), ::tolower);

        std::vector<const NodeTypeInfo*> matchingTypes;
        for (const auto& [name, info] : NodeRegistry::Instance().GetAllTypes()) {
            // Check family filter
            if (m_selectedFamilyIndex == 1 && info.family != NodeFamily::TexOp) continue;
            if (m_selectedFamilyIndex == 2 && info.family != NodeFamily::ChanOp) continue;
            if (m_selectedFamilyIndex == 3 && info.family != NodeFamily::GeomOp) continue;
            if (m_selectedFamilyIndex == 4 && info.family != NodeFamily::MatOp) continue;
            if (m_selectedFamilyIndex == 5 && info.family != NodeFamily::DataOp) continue;
            if (m_selectedFamilyIndex == 6 && info.family != NodeFamily::Comp) continue;

            // Check query
            if (!query.empty()) {
                std::string lowerName = info.typeName;
                std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
                if (lowerName.find(query) == std::string::npos) continue;
            }

            matchingTypes.push_back(&info);
        }

        // List matching nodes
        ImGui::BeginChild("OpList", ImVec2(0.0f, -36.0f), true);
        const NodeTypeInfo* selectedToInstantiate = nullptr;

        for (size_t i = 0; i < matchingTypes.size(); ++i) {
            const auto* info = matchingTypes[i];
            ImVec4 famColor = EditorTheme::GetFamilyColor(info->family);

            ImGui::PushID(static_cast<int>(i));
            ImGui::PushStyleColor(ImGuiCol_Text, famColor);
            ImGui::Bullet();
            ImGui::PopStyleColor();
            ImGui::SameLine();

            bool isSelected = (static_cast<int>(i) == m_highlightedItemIndex);
            if (ImGui::Selectable(info->typeName.c_str(), isSelected)) {
                selectedToInstantiate = info;
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("%s\nCategory: %s\n%s", info->typeName.c_str(), info->category.c_str(), info->description.c_str());
            }
            ImGui::PopID();
        }
        ImGui::EndChild();

        // Keyboard arrow navigation and enter key
        if (ImGui::IsKeyPressed(ImGuiKey_DownArrow)) {
            m_highlightedItemIndex = std::min(m_highlightedItemIndex + 1, static_cast<int>(matchingTypes.size()) - 1);
        }
        if (ImGui::IsKeyPressed(ImGuiKey_UpArrow)) {
            m_highlightedItemIndex = std::max(m_highlightedItemIndex - 1, 0);
        }
        if (ImGui::IsKeyPressed(ImGuiKey_Enter) && !matchingTypes.empty() && m_highlightedItemIndex < static_cast<int>(matchingTypes.size())) {
            selectedToInstantiate = matchingTypes[m_highlightedItemIndex];
        }

        // Instantiate
        if (selectedToInstantiate) {
            float zoom = m_ctx->GetCanvasZoom();
            const glm::vec2& pan = m_ctx->GetCanvasPan();
            glm::vec2 canvasSpawnPos(
                (m_ctx->GetOpPalettePos().x - 100.0f) / zoom - pan.x,
                (m_ctx->GetOpPalettePos().y - 100.0f) / zoom - pan.y
            );

            std::string opName = selectedToInstantiate->typeName;
            if (opName.size() > 2 && opName.substr(opName.size() - 2) == "Op") {
                opName = opName.substr(0, opName.size() - 2);
            }
            std::string instName = opName + "1";

            m_ctx->GetUndoManager().ExecuteCommand(
                std::make_unique<CreateNodeCommand>(m_ctx, selectedToInstantiate->typeName, instName, canvasSpawnPos)
            );

            m_ctx->CloseOpPalette();
            ImGui::CloseCurrentPopup();
        }

        if (ImGui::Button("Cancel", ImVec2(-1.0f, 0.0f)) || ImGui::IsKeyPressed(ImGuiKey_Escape)) {
            m_ctx->CloseOpPalette();
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

} // namespace nf::ui
