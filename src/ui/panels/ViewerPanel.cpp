#include "ViewerPanel.h"
#include "../EditorTheme.h"
#include "../../operators/tex/TexOp.h"
#include "../../graph/Pin.h"
#include <imgui_impl_vulkan.h>
#include <algorithm>

namespace nf::ui {

ViewerPanel::ViewerPanel(EditorContext* ctx)
    : m_ctx(ctx) {
}

void ViewerPanel::Render() {
    ImGui::Begin("Viewer");

    Node* node = m_ctx->GetActiveNode();
    if (!node) {
        ImGui::TextDisabled("Select an operator to preview output");
        ImGui::End();
        return;
    }

    // Top Viewer Controls Toolbar
    const char* channels[] = { "RGBA", "RGB", "R", "G", "B", "A" };
    ImGui::SetNextItemWidth(70.0f);
    ImGui::Combo("##Channels", &m_channelMode, channels, IM_ARRAYSIZE(channels));

    ImGui::SameLine();
    ImGui::Checkbox("Fit", &m_fitToWindow);

    ImGui::SameLine();
    if (ImGui::Button("1:1")) {
        m_fitToWindow = false;
        m_viewerZoom = 1.0f;
    }

    ImGui::SameLine();
    ImGui::TextDisabled("| %s", node->GetName().c_str());

    ImGui::Separator();

    // Render based on node family
    if (node->GetFamily() == NodeFamily::TexOp) {
        RenderTexViewer(node);
    } else if (node->GetFamily() == NodeFamily::ChanOp) {
        RenderChanViewer(node);
    } else {
        RenderDataViewer(node);
    }

    ImGui::End();
}

void ViewerPanel::RenderTexViewer(Node* node) {
    auto* texOp = dynamic_cast<TexOp*>(node);
    if (!texOp) return;

    auto tex = texOp->GetOutputTexture();
    ImVec2 avail = ImGui::GetContentRegionAvail();
    if (avail.x < 10.0f || avail.y < 10.0f) return;

    if (!tex) {
        ImGui::TextDisabled("Texture not cooked or empty");
        return;
    }

    uint32_t tw = tex->GetWidth();
    uint32_t th = tex->GetHeight();

    ImVec2 displaySize;
    if (m_fitToWindow) {
        float aspect = static_cast<float>(tw) / static_cast<float>(th);
        if (avail.x / avail.y > aspect) {
            displaySize = ImVec2(avail.y * aspect, avail.y);
        } else {
            displaySize = ImVec2(avail.x, avail.x / aspect);
        }
    } else {
        displaySize = ImVec2(static_cast<float>(tw) * m_viewerZoom, static_cast<float>(th) * m_viewerZoom);
    }

    // Center image in viewport
    ImVec2 cursor = ImGui::GetCursorScreenPos();
    ImVec2 centerPos(
        cursor.x + (avail.x - displaySize.x) * 0.5f,
        cursor.y + (avail.y - displaySize.y) * 0.5f
    );

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    drawList->AddRectFilled(centerPos, ImVec2(centerPos.x + displaySize.x, centerPos.y + displaySize.y), IM_COL32(10, 12, 16, 255));
    drawList->AddRect(centerPos, ImVec2(centerPos.x + displaySize.x, centerPos.y + displaySize.y), EditorTheme::ColBorder, 0.0f, 0, 1.0f);

    // Live descriptor texture preview
    if (tex->GetImageView() != VK_NULL_HANDLE && tex->GetSampler() != VK_NULL_HANDLE) {
        VkDescriptorSet descSet = ImGui_ImplVulkan_AddTexture(tex->GetSampler(), tex->GetImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        if (descSet != VK_NULL_HANDLE) {
            drawList->AddImage(reinterpret_cast<ImTextureID>(descSet), centerPos, ImVec2(centerPos.x + displaySize.x, centerPos.y + displaySize.y));
        }
    } else {
        drawList->AddText(ImVec2(centerPos.x + 10.0f, centerPos.y + 10.0f), IM_COL32(160, 170, 185, 255), "Live Vulkan Texture");
    }

    // Resolution and Format Footer
    ImVec2 mousePos = ImGui::GetMousePos();
    if (mousePos.x >= centerPos.x && mousePos.x <= centerPos.x + displaySize.x &&
        mousePos.y >= centerPos.y && mousePos.y <= centerPos.y + displaySize.y) {
        float u = (mousePos.x - centerPos.x) / displaySize.x;
        float v = (mousePos.y - centerPos.y) / displaySize.y;
        int px = static_cast<int>(u * static_cast<float>(tw));
        int py = static_cast<int>(v * static_cast<float>(th));

        ImGui::SetCursorScreenPos(ImVec2(cursor.x + 10.0f, cursor.y + avail.y - 25.0f));
        ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "%ux%u | UV: (%.3f, %.3f) | Pixel: (%d, %d)", tw, th, u, v, px, py);
    } else {
        ImGui::SetCursorScreenPos(ImVec2(cursor.x + 10.0f, cursor.y + avail.y - 25.0f));
        ImGui::TextDisabled("%ux%u RGBA8", tw, th);
    }
}

void ViewerPanel::RenderChanViewer(Node* node) {
    ImVec2 avail = ImGui::GetContentRegionAvail();
    ImGui::Text("Channel Outputs:");
    for (const auto& pin : node->GetOutputPins()) {
        const auto& val = pin->GetValue();
        if (val.Is<float>()) {
            float f = val.Get<float>();
            ImGui::Text("%s: %.4f", pin->GetName().c_str(), f);
            ImGui::ProgressBar((f + 1.0f) * 0.5f, ImVec2(avail.x * 0.7f, 16.0f));
        } else if (val.Is<glm::vec3>()) {
            auto v = val.Get<glm::vec3>();
            ImGui::Text("%s: (%.3f, %.3f, %.3f)", pin->GetName().c_str(), v.x, v.y, v.z);
        } else if (val.Is<glm::vec4>()) {
            auto v = val.Get<glm::vec4>();
            ImGui::Text("%s: (%.3f, %.3f, %.3f, %.3f)", pin->GetName().c_str(), v.x, v.y, v.z, v.w);
        }
    }
}

void ViewerPanel::RenderDataViewer(Node* node) {
    ImGui::Text("Data / Script Outputs:");
    for (const auto& pin : node->GetOutputPins()) {
        const auto& val = pin->GetValue();
        if (val.Is<std::string>()) {
            ImGui::BulletText("%s: %s", pin->GetName().c_str(), val.Get<std::string>().c_str());
        } else {
            ImGui::BulletText("%s", pin->GetName().c_str());
        }
    }
}

} // namespace nf::ui
