#include "PerformanceHUD.h"
#include "../EditorContext.h"
#include "../../profiling/CookProfiler.h"
#include "../../gpu/GpuTimerPool.h"
#include <imgui.h>

namespace nf::ui {

PerformanceHUD::PerformanceHUD(EditorContext* ctx)
    : m_ctx(ctx) {
}

void PerformanceHUD::Render(bool* pOpen) {
    if (pOpen && !*pOpen) return;
    if (!m_visible && (!pOpen || !*pOpen)) return;

    const float PADDING = 12.0f;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 workPos = viewport->WorkPos;
    ImVec2 workSize = viewport->WorkSize;
    ImVec2 windowPos(workPos.x + workSize.x - PADDING, workPos.y + PADDING);
    ImVec2 windowPosPivot(1.0f, 0.0f);

    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, windowPosPivot);
    ImGui::SetNextWindowBgAlpha(0.65f); // Translucent background

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration |
                            ImGuiWindowFlags_AlwaysAutoResize |
                            ImGuiWindowFlags_NoSavedSettings |
                            ImGuiWindowFlags_NoFocusOnAppearing |
                            ImGuiWindowFlags_NoNav;

    if (ImGui::Begin("Performance HUD##overlay", pOpen ? pOpen : &m_visible, flags)) {
        auto& profiler = CookProfiler::Instance();
        auto& gpuTimer = GpuTimerPool::Instance();
        auto frameData = profiler.GetLatestFrameData();

        // FPS Header with Color Code
        ImVec4 fpsColor = (frameData.currentFps >= 55.0f) ? ImVec4(0.3f, 1.0f, 0.3f, 1.0f) :
                          (frameData.currentFps >= 28.0f) ? ImVec4(1.0f, 0.85f, 0.2f, 1.0f) :
                                                            ImVec4(1.0f, 0.3f, 0.3f, 1.0f);

        ImGui::TextColored(fpsColor, "%.1f FPS", frameData.currentFps);
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "(%.2f ms)", frameData.frameDeltaMs);

        // Frame timing plot
        const auto& fpsHistory = profiler.GetFpsHistory();
        ImGui::PlotLines("##hud_fps", fpsHistory.data(), static_cast<int>(fpsHistory.size()), 0, nullptr, 0.0f, 75.0f, ImVec2(160, 24));

        ImGui::Separator();
        ImGui::Text("Graph CPU: %.3f ms", frameData.totalGraphCpuMs);
        ImGui::Text("GPU Passes: %.3f ms", gpuTimer.GetTotalGpuTimeMs());
        ImGui::TextColored(ImVec4(0.5f, 0.8f, 1.0f, 1.0f), "Active Nodes: %zu", frameData.activeNodeCount);
    }
    ImGui::End();
}

} // namespace nf::ui
