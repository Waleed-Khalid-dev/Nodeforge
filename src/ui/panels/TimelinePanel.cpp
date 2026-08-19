#include "TimelinePanel.h"
#include <imgui.h>

namespace nf::ui {

TimelinePanel::TimelinePanel(EditorContext* ctx)
    : m_ctx(ctx) {
}

void TimelinePanel::Render() {
    ImGui::Begin("Timeline", nullptr, ImGuiWindowFlags_NoScrollbar);

    bool isPlaying = m_ctx->IsPlaying();
    uint64_t currentFrame = m_ctx->GetCurrentFrame();

    // Transport buttons
    if (ImGui::Button(isPlaying ? "Pause" : "Play", ImVec2(55.0f, 0.0f))) {
        m_ctx->SetPlaying(!isPlaying);
    }
    ImGui::SameLine();
    if (ImGui::Button("|<", ImVec2(28.0f, 0.0f))) {
        m_ctx->SetCurrentFrame(static_cast<uint64_t>(m_startFrame));
    }
    ImGui::SameLine();
    if (ImGui::Button("<", ImVec2(24.0f, 0.0f))) {
        if (currentFrame > static_cast<uint64_t>(m_startFrame)) {
            m_ctx->SetCurrentFrame(currentFrame - 1);
        }
    }
    ImGui::SameLine();
    if (ImGui::Button(">", ImVec2(24.0f, 0.0f))) {
        if (currentFrame < static_cast<uint64_t>(m_endFrame)) {
            m_ctx->SetCurrentFrame(currentFrame + 1);
        }
    }

    ImGui::SameLine();
    ImGui::Text("Frame: %llu", currentFrame);

    ImGui::SameLine();
    double timeSec = static_cast<double>(currentFrame) / static_cast<double>(m_ctx->GetPlaybackFps());
    ImGui::TextDisabled("(%.2fs @ %.0f FPS)", timeSec, m_ctx->GetPlaybackFps());

    ImGui::SameLine(320.0f);
    ImGui::SetNextItemWidth(-1.0f);

    int frameSlider = static_cast<int>(currentFrame);
    if (ImGui::SliderInt("##FrameScrubber", &frameSlider, m_startFrame, m_endFrame)) {
        m_ctx->SetCurrentFrame(static_cast<uint64_t>(frameSlider));
    }

    ImGui::End();
}

} // namespace nf::ui
