#include "ConsolePanel.h"
#include "../../python/PythonEngine.h"
#include <imgui.h>
#include <cstring>

namespace nf::ui {

ConsolePanel::ConsolePanel(EditorContext* ctx)
    : m_ctx(ctx) {
    AddLog(LogEntry::Level::Info, "NodeForge Studio v0.1.0 Ready.");
    AddLog(LogEntry::Level::Info, "Vulkan 1.3 Dynamic Rendering & Python 3.11 Runtime Initialized.");
}

void ConsolePanel::AddLog(LogEntry::Level level, const std::string& msg) {
    m_logs.push_back({ level, msg });
    if (m_logs.size() > 500) {
        m_logs.erase(m_logs.begin());
    }
}

void ConsolePanel::Clear() {
    m_logs.clear();
}

void ConsolePanel::Render() {
    ImGui::Begin("Console / Python");

    if (ImGui::Button("Clear")) {
        Clear();
    }
    ImGui::SameLine();
    ImGui::Checkbox("Auto-Scroll", &m_autoScroll);

    ImGui::Separator();

    // Log Output Window
    float footerHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    ImGui::BeginChild("LogScrollingRegion", ImVec2(0.0f, -footerHeight), false, ImGuiWindowFlags_HorizontalScrollbar);

    for (const auto& entry : m_logs) {
        ImVec4 col;
        switch (entry.level) {
            case LogEntry::Level::Info:  col = ImVec4(0.85f, 0.88f, 0.92f, 1.0f); break;
            case LogEntry::Level::Warn:  col = ImVec4(0.95f, 0.75f, 0.20f, 1.0f); break;
            case LogEntry::Level::Error: col = ImVec4(0.95f, 0.30f, 0.35f, 1.0f); break;
        }
        ImGui::PushStyleColor(ImGuiCol_Text, col);
        ImGui::TextUnformatted(entry.text.c_str());
        ImGui::PopStyleColor();
    }

    if (m_autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(1.0f);
    }
    ImGui::EndChild();

    ImGui::Separator();

    // Python Command Line Input
    ImGui::TextColored(ImVec4(0.35f, 0.65f, 0.95f, 1.0f), ">>>");
    ImGui::SameLine();

    ImGui::SetNextItemWidth(-1.0f);
    bool reclaimFocus = false;
    ImGuiInputTextFlags inputFlags = ImGuiInputTextFlags_EnterReturnsTrue;

    if (ImGui::InputText("##ConsoleInput", m_inputBuffer, sizeof(m_inputBuffer), inputFlags)) {
        std::string cmd = m_inputBuffer;
        if (!cmd.empty()) {
            AddLog(LogEntry::Level::Info, ">>> " + cmd);
            m_history.push_back(cmd);
            m_historyPos = -1;

            std::string err;
            if (PythonEngine::Instance().ExecuteString(cmd, &err)) {
                // Command executed successfully
            } else {
                AddLog(LogEntry::Level::Error, err);
            }
            m_inputBuffer[0] = '\0';
        }
        reclaimFocus = true;
    }

    if (reclaimFocus) {
        ImGui::SetKeyboardFocusHere(-1);
    }

    ImGui::End();
}

} // namespace nf::ui
