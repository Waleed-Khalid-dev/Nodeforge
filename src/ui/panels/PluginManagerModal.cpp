#include "PluginManagerModal.h"
#include "../../plugin/PluginManager.h"
#include "../EditorContext.h"
#include <imgui.h>
#include <fmt/format.h>

namespace nf::ui {

PluginManagerModal::PluginManagerModal(EditorContext* ctx)
    : m_ctx(ctx) {
}

void PluginManagerModal::Render(bool* pOpen) {
    if (pOpen && !*pOpen) return;

    ImGui::SetNextWindowSize(ImVec2(750, 480), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Plugin Manager", pOpen, ImGuiWindowFlags_None)) {
        ImGui::End();
        return;
    }

    auto& pm = PluginManager::Instance();

    // ─── Header Action Toolbar ───────────────────────────────────────────────
    ImGui::Text("Loaded Plugins: %zu", pm.GetLoadedPluginCount());
    ImGui::SameLine();
    if (ImGui::Button("Scan & Load New")) {
        uint32_t count = pm.ScanAndLoadPlugins();
        fmt::format_to_n(m_statusMessage, sizeof(m_statusMessage) - 1, "Scanned: {} new plugins loaded", count);
    }
    ImGui::SameLine();
    if (ImGui::Button("Reload All Plugins")) {
        uint32_t count = pm.ReloadAllPlugins();
        fmt::format_to_n(m_statusMessage, sizeof(m_statusMessage) - 1, "Reloaded: {} plugins", count);
    }

    if (m_statusMessage[0] != '\0') {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.2f, 0.9f, 0.4f, 1.0f), "%s", m_statusMessage);
    }

    ImGui::Separator();

    // ─── Search Paths Collapsing Header ──────────────────────────────────────
    if (ImGui::CollapsingHeader("Discovery Search Paths", ImGuiTreeNodeFlags_DefaultOpen)) {
        auto paths = pm.GetSearchPaths();
        for (const auto& p : paths) {
            ImGui::BulletText("%s", p.string().c_str());
        }

        ImGui::PushItemWidth(350);
        ImGui::InputTextWithHint("##AddSearchPath", "Add custom plugin directory path...", m_newSearchPathBuffer, sizeof(m_newSearchPathBuffer));
        ImGui::PopItemWidth();
        ImGui::SameLine();
        if (ImGui::Button("Add Directory") && m_newSearchPathBuffer[0] != '\0') {
            pm.AddSearchPath(std::filesystem::path(m_newSearchPathBuffer));
            m_newSearchPathBuffer[0] = '\0';
        }
    }

    ImGui::Separator();

    // ─── Loaded Plugins Table ────────────────────────────────────────────────
    ImGui::TextDisabled("Registered Dynamic Operators & Modules");

    auto plugins = pm.GetLoadedPlugins();
    if (plugins.empty()) {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No external plugins loaded. Place .dll plugins in one of the search paths above.");
    } else {
        if (ImGui::BeginTable("PluginsTable", 6, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY)) {
            ImGui::TableSetupColumn("Plugin Name", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Version", ImGuiTableColumnFlags_WidthFixed, 65.0f);
            ImGui::TableSetupColumn("Author", ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableSetupColumn("Operators", ImGuiTableColumnFlags_WidthFixed, 75.0f);
            ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed, 80.0f);
            ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed, 130.0f);
            ImGui::TableHeadersRow();

            for (size_t i = 0; i < plugins.size(); ++i) {
                const auto& p = plugins[i];
                ImGui::TableNextRow();

                // 1. Name & Description tooltip
                ImGui::TableNextColumn();
                ImGui::Text("%s", p.name.c_str());
                if (!p.description.empty() && ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("%s\nPath: %s", p.description.c_str(), p.filePath.string().c_str());
                }

                // 2. Version
                ImGui::TableNextColumn();
                ImGui::Text("%s", p.versionString.c_str());

                // 3. Author
                ImGui::TableNextColumn();
                ImGui::Text("%s", p.author.c_str());

                // 4. Operators Count
                ImGui::TableNextColumn();
                ImGui::Text("%zu ops", p.operators.size());

                // 5. Status
                ImGui::TableNextColumn();
                if (p.status == PluginStatus::Loaded) {
                    ImGui::TextColored(ImVec4(0.2f, 0.9f, 0.3f, 1.0f), "Active");
                } else if (p.status == PluginStatus::IncompatibleABI) {
                    ImGui::TextColored(ImVec4(0.9f, 0.3f, 0.2f, 1.0f), "Bad ABI");
                } else {
                    ImGui::TextColored(ImVec4(0.8f, 0.6f, 0.1f, 1.0f), "Error");
                }

                // 6. Actions
                ImGui::TableNextColumn();
                ImGui::PushID(static_cast<int>(i));
                if (ImGui::SmallButton("Reload")) {
                    pm.ReloadPlugin(p.name);
                    fmt::format_to_n(m_statusMessage, sizeof(m_statusMessage) - 1, "Reloaded {}", p.name);
                }
                ImGui::SameLine();
                if (ImGui::SmallButton("Unload")) {
                    pm.UnloadPlugin(p.name);
                    fmt::format_to_n(m_statusMessage, sizeof(m_statusMessage) - 1, "Unloaded {}", p.name);
                }
                ImGui::PopID();
            }
            ImGui::EndTable();
        }
    }

    ImGui::End();
}

} // namespace nf::ui
