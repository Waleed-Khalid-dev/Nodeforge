#include "BreadcrumbBar.h"
#include "../EditorTheme.h"

namespace nf::ui {

BreadcrumbBar::BreadcrumbBar(EditorContext* ctx)
    : m_ctx(ctx) {
}

void BreadcrumbBar::Render() {
    if (!m_ctx) return;

    const auto& stack = m_ctx->GetNavStack();
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.0f, 4.0f));

    // Up button
    if (stack.size() > 1) {
        if (ImGui::Button("^ Up", ImVec2(0.0f, 20.0f))) {
            m_ctx->ExitContainer();
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Exit container (Hotkey: U / Backspace)");
        }
        ImGui::SameLine();
    }

    // Breadcrumb path buttons
    for (size_t i = 0; i < stack.size(); ++i) {
        bool isCurrent = (i == stack.size() - 1);
        const auto& level = stack[i];

        ImGui::PushID(static_cast<int>(i));
        if (isCurrent) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.25f, 0.35f, 0.50f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        } else {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.17f, 0.22f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.75f, 0.85f, 1.0f));
        }

        if (ImGui::Button(level.name.c_str(), ImVec2(0.0f, 20.0f))) {
            m_ctx->NavigateToLevel(i);
        }
        ImGui::PopStyleColor(2);
        ImGui::PopID();

        if (i < stack.size() - 1) {
            ImGui::SameLine();
            ImGui::TextDisabled(">");
            ImGui::SameLine();
        }
    }

    // Right-aligned network info
    Graph* g = m_ctx->GetGraph();
    if (g) {
        ImVec2 avail = ImGui::GetContentRegionAvail();
        char buf[64];
        std::snprintf(buf, sizeof(buf), "%zu Nodes | %zu Wires%s", 
                      g->GetNodes().size(), g->GetWires().size(),
                      m_ctx->IsDirty() ? " *" : "");
        float textWidth = ImGui::CalcTextSize(buf).x;
        if (avail.x > textWidth + 20.0f) {
            ImGui::SameLine(ImGui::GetWindowWidth() - textWidth - 25.0f);
            ImGui::TextDisabled("%s", buf);
        }
    }

    ImGui::PopStyleVar(2);
    ImGui::Separator();
}

} // namespace nf::ui
