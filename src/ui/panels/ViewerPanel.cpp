#include "ViewerPanel.h"
#include "../EditorTheme.h"
#include "../../operators/tex/TexOp.h"
#include "../../core/ChannelBuffer.h"
#include "../../graph/Pin.h"
#include <imgui_impl_vulkan.h>
#include <algorithm>
#include <cmath>

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
    if (node->GetFamily() == NodeFamily::TexOp) {
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
    }

    ImGui::TextDisabled("%s | %s", node->GetTypeName().c_str(), node->GetName().c_str());
    ImGui::Separator();

    // Render based on node family
    if (node->GetFamily() == NodeFamily::TexOp) {
        RenderTexViewer(node);
    } else if (node->GetFamily() == NodeFamily::ChanOp) {
        RenderChanViewer(node);
    } else if (node->GetFamily() == NodeFamily::GeomOp) {
        RenderGeomViewer(node);
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
    const ChannelBuffer* chanBuf = nullptr;
    for (const auto& pin : node->GetOutputPins()) {
        if (pin->GetValue().Is<ChannelBuffer>()) {
            chanBuf = &pin->GetValue().Get<ChannelBuffer>();
            break;
        }
    }

    if (!chanBuf || chanBuf->IsEmpty()) {
        ImGui::TextDisabled("Channel buffer empty or not cooked");
        return;
    }

    size_t numChans = chanBuf->GetChannelCount();
    size_t numSamples = chanBuf->GetSampleCount();
    float sampleRate = chanBuf->GetSampleRate();
    float durationSec = (sampleRate > 1e-6f) ? (static_cast<float>(numSamples) / sampleRate) : 0.0f;

    // Header Toolbar
    ImGui::TextColored(ImVec4(0.3f, 0.85f, 0.95f, 1.0f), "%zu Channels | %zu Samples | %.1f Hz | %.3fs",
        numChans, numSamples, sampleRate, durationSec);

    // Draggable channel badges for drag-and-drop parameter binding
    ImGui::TextDisabled("Drag channel to bind parameter:");
    for (size_t c = 0; c < numChans; ++c) {
        ImGui::SameLine();
        const std::string& cname = chanBuf->GetChannelNames()[c];
        ImGui::PushID(static_cast<int>(c));
        ImGui::SmallButton(cname.c_str());
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
            std::string payload = node->GetName() + "/" + cname;
            ImGui::SetDragDropPayload("NF_CHANNEL_BIND", payload.c_str(), payload.size() + 1);
            ImGui::Text("Bind '%s.%s'", node->GetName().c_str(), cname.c_str());
            ImGui::EndDragDropSource();
        }
        ImGui::PopID();
    }

    ImVec2 avail = ImGui::GetContentRegionAvail();
    if (avail.x < 40.0f || avail.y < 40.0f) return;

    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    ImVec2 canvasSize = ImVec2(avail.x, avail.y - 10.0f);
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    // Background & Border
    drawList->AddRectFilled(canvasPos, ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y), IM_COL32(12, 14, 18, 255));
    drawList->AddRect(canvasPos, ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y), EditorTheme::ColBorder, 4.0f);

    // Dynamic Range Computation
    float minVal = -1.0f;
    float maxVal = 1.0f;
    for (size_t c = 0; c < numChans; ++c) {
        const float* d = chanBuf->GetChannelData(c);
        for (size_t s = 0; s < numSamples; ++s) {
            minVal = std::min(minVal, d[s]);
            maxVal = std::max(maxVal, d[s]);
        }
    }
    float rangePadding = (maxVal - minVal) * 0.1f;
    minVal -= rangePadding;
    maxVal += rangePadding;
    float valRange = (std::abs(maxVal - minVal) > 1e-6f) ? (maxVal - minVal) : 1.0f;

    // Grid lines (Zero Line, Top, Bottom, Vertical Division)
    float zeroY = canvasPos.y + canvasSize.y * (1.0f - (0.0f - minVal) / valRange);
    if (zeroY >= canvasPos.y && zeroY <= canvasPos.y + canvasSize.y) {
        drawList->AddLine(ImVec2(canvasPos.x, zeroY), ImVec2(canvasPos.x + canvasSize.x, zeroY), IM_COL32(60, 70, 85, 200), 1.5f);
        drawList->AddText(ImVec2(canvasPos.x + 5.0f, zeroY - 14.0f), IM_COL32(120, 135, 150, 200), "0.0");
    }

    // Vertical time subdivision lines
    int vDivs = 8;
    for (int i = 1; i < vDivs; ++i) {
        float x = canvasPos.x + (canvasSize.x / static_cast<float>(vDivs)) * static_cast<float>(i);
        drawList->AddLine(ImVec2(x, canvasPos.y), ImVec2(x, canvasPos.y + canvasSize.y), IM_COL32(30, 36, 46, 150), 1.0f);
    }

    // Waveform Palette (Distinct, readable modern hues)
    const ImU32 chanColors[] = {
        IM_COL32(0, 220, 255, 255),   // Cyan
        IM_COL32(255, 170, 0, 255),   // Amber
        IM_COL32(0, 230, 130, 255),   // Emerald
        IM_COL32(255, 75, 110, 255),  // Rose
        IM_COL32(175, 115, 255, 255), // Violet
        IM_COL32(255, 230, 0, 255)    // Yellow
    };
    const size_t numColors = sizeof(chanColors) / sizeof(chanColors[0]);

    // Draw Multi-Channel Waveforms
    for (size_t c = 0; c < numChans; ++c) {
        const float* d = chanBuf->GetChannelData(c);
        ImU32 col = chanColors[c % numColors];

        if (numSamples == 1) {
            // Single point representation (level bar or point)
            float y = canvasPos.y + canvasSize.y * (1.0f - (d[0] - minVal) / valRange);
            drawList->AddCircleFilled(ImVec2(canvasPos.x + canvasSize.x * 0.5f, y), 5.0f, col);
            drawList->AddLine(ImVec2(canvasPos.x, y), ImVec2(canvasPos.x + canvasSize.x, y), col, 1.5f);
        } else {
            // Polyline representation
            float dx = canvasSize.x / static_cast<float>(numSamples - 1);
            for (size_t s = 0; s < numSamples - 1; ++s) {
                float x0 = canvasPos.x + static_cast<float>(s) * dx;
                float y0 = canvasPos.y + canvasSize.y * (1.0f - (d[s] - minVal) / valRange);
                float x1 = canvasPos.x + static_cast<float>(s + 1) * dx;
                float y1 = canvasPos.y + canvasSize.y * (1.0f - (d[s + 1] - minVal) / valRange);
                drawList->AddLine(ImVec2(x0, y0), ImVec2(x1, y1), col, 2.0f);
            }
        }
    }

    // Hover Inspection Tooltip
    ImVec2 mousePos = ImGui::GetMousePos();
    if (mousePos.x >= canvasPos.x && mousePos.x <= canvasPos.x + canvasSize.x &&
        mousePos.y >= canvasPos.y && mousePos.y <= canvasPos.y + canvasSize.y) {
        drawList->AddLine(ImVec2(mousePos.x, canvasPos.y), ImVec2(mousePos.x, canvasPos.y + canvasSize.y), IM_COL32(255, 255, 255, 120), 1.0f);

        float normX = (mousePos.x - canvasPos.x) / canvasSize.x;
        size_t sampleIdx = std::min(numSamples - 1, static_cast<size_t>(normX * static_cast<float>(numSamples)));

        ImGui::BeginTooltip();
        ImGui::Text("Sample #%zu (%.4fs)", sampleIdx, static_cast<float>(sampleIdx) / sampleRate);
        ImGui::Separator();
        for (size_t c = 0; c < numChans; ++c) {
            float val = chanBuf->GetSample(c, sampleIdx);
            ImU32 col = chanColors[c % numColors];
            ImVec4 colVec = ImGui::ColorConvertU32ToFloat4(col);
            ImGui::TextColored(colVec, "%s: %.4f", chanBuf->GetChannelNames()[c].c_str(), val);
        }
        ImGui::EndTooltip();
    }
}

void ViewerPanel::RenderDataViewer(Node* node) {
    const DataTable* dataTable = nullptr;
    for (const auto& pin : node->GetOutputPins()) {
        if (pin->GetValue().Is<DataTable>()) {
            dataTable = &pin->GetValue().Get<DataTable>();
            break;
        }
    }

    if (!dataTable || dataTable->IsEmpty()) {
        ImGui::TextDisabled("Table is empty or not cooked");
        return;
    }

    size_t numRows = dataTable->GetRowCount();
    size_t numCols = dataTable->GetColumnCount();
    const auto& headers = dataTable->GetColumnHeaders();

    // Header Toolbar
    ImGui::TextColored(ImVec4(0.95f, 0.55f, 0.2f, 1.0f), "Table: %zu Rows x %zu Columns", numRows, numCols);
    ImGui::SameLine();
    if (ImGui::SmallButton("Copy CSV")) {
        std::string csvStr = dataTable->ToCSV();
        ImGui::SetClipboardText(csvStr.c_str());
    }

    // Interactive ImGui Table Spreadsheet
    ImGuiTableFlags flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable |
                            ImGuiTableFlags_Hideable | ImGuiTableFlags_RowBg |
                            ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV |
                            ImGuiTableFlags_BordersH | ImGuiTableFlags_ScrollY |
                            ImGuiTableFlags_ScrollX;

    ImVec2 avail = ImGui::GetContentRegionAvail();
    if (ImGui::BeginTable("##DataTableGrid", static_cast<int>(numCols + 1), flags, avail)) {
        // Setup row index header
        ImGui::TableSetupColumn("#", ImGuiTableColumnFlags_WidthFixed, 35.0f);
        for (size_t c = 0; c < numCols; ++c) {
            std::string headerName = (c < headers.size()) ? headers[c] : ("col" + std::to_string(c));
            ImGui::TableSetupColumn(headerName.c_str(), ImGuiTableColumnFlags_WidthStretch);
        }
        ImGui::TableHeadersRow();

        // Virtualized rows with clipper
        ImGuiListClipper clipper;
        clipper.Begin(static_cast<int>(numRows));
        while (clipper.Step()) {
            for (int r = clipper.DisplayStart; r < clipper.DisplayEnd; ++r) {
                ImGui::TableNextRow();
                // Row number
                ImGui::TableSetColumnIndex(0);
                ImGui::TextDisabled("%d", r);

                // Row cells
                for (size_t c = 0; c < numCols; ++c) {
                    ImGui::TableSetColumnIndex(static_cast<int>(c + 1));
                    std::string cellVal = dataTable->GetCell(static_cast<size_t>(r), c);
                    ImGui::TextUnformatted(cellVal.c_str());
                }
            }
        }
        ImGui::EndTable();
    }
}

void ViewerPanel::RenderGeomViewer(Node* node) {
    const GeometryData* geom = nullptr;
    for (const auto& pin : node->GetOutputPins()) {
        if (pin->GetValue().Is<GeometryData>()) {
            geom = &pin->GetValue().Get<GeometryData>();
            break;
        }
    }

    if (!geom || geom->IsEmpty()) {
        ImGui::TextDisabled("Geometry is empty or not cooked");
        return;
    }

    size_t vertCount = geom->GetVertexCount();
    size_t triCount = geom->GetTriangleCount();
    BoundingBox bbox = geom->ComputeBounds();
    glm::vec3 bsize = bbox.GetSize();

    // Toolbar Header
    ImGui::TextColored(ImVec4(0.3f, 0.9f, 0.5f, 1.0f), "3D Mesh: %zu Verts | %zu Tris | Bounds: (%.2f, %.2f, %.2f)",
        vertCount, triCount, bsize.x, bsize.y, bsize.z);

    ImGui::SameLine();
    ImGui::Checkbox("Grid", &m_showGrid);
    ImGui::SameLine();
    ImGui::Checkbox("Wireframe", &m_showWireframe);

    // Interactive 3D Viewport Canvas
    ImVec2 avail = ImGui::GetContentRegionAvail();
    avail.y = std::max(100.0f, avail.y);
    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    ImVec2 canvasSize = avail;

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    drawList->AddRectFilled(canvasPos, ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y), IM_COL32(12, 16, 22, 255));
    drawList->AddRect(canvasPos, ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y), EditorTheme::ColBorder, 0.0f, 0, 1.0f);

    // Mouse Interaction for 3D Camera Orbit
    ImGui::InvisibleButton("##GeomViewportInput", canvasSize);
    bool isHovered = ImGui::IsItemHovered();
    ImGuiIO& io = ImGui::GetIO();

    if (isHovered && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        m_orbitYaw += io.MouseDelta.x * 0.5f;
        m_orbitPitch = std::clamp(m_orbitPitch + io.MouseDelta.y * 0.5f, -89.0f, 89.0f);
    }
    if (isHovered && io.MouseWheel != 0.0f) {
        m_orbitDistance = std::clamp(m_orbitDistance - io.MouseWheel * 0.3f, 0.5f, 100.0f);
    }

    // Compute View and Projection Matrices
    float yawRad = glm::radians(m_orbitYaw);
    float pitchRad = glm::radians(m_orbitPitch);
    glm::vec3 eye(
        m_orbitDistance * std::cos(pitchRad) * std::sin(yawRad),
        m_orbitDistance * std::sin(pitchRad),
        m_orbitDistance * std::cos(pitchRad) * std::cos(yawRad)
    );
    glm::mat4 view = glm::lookAt(eye, glm::vec3(0.0f), glm::vec3(0, 1, 0));
    float aspect = canvasSize.x / canvasSize.y;
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 1000.0f);
    glm::mat4 vp = proj * view;

    auto project3D = [&](const glm::vec3& p3, ImVec2& outP2) -> bool {
        glm::vec4 clip = vp * glm::vec4(p3, 1.0f);
        if (clip.w <= 0.001f) return false;
        glm::vec3 ndc = glm::vec3(clip) / clip.w;
        if (ndc.z < -1.0f || ndc.z > 1.0f) return false;

        outP2.x = canvasPos.x + (ndc.x * 0.5f + 0.5f) * canvasSize.x;
        outP2.y = canvasPos.y + (-ndc.y * 0.5f + 0.5f) * canvasSize.y;
        return true;
    };

    // Draw Floor Grid
    if (m_showGrid) {
        constexpr int gridLines = 10;
        constexpr float gridExtent = 2.0f;
        for (int i = -gridLines; i <= gridLines; ++i) {
            float coord = (static_cast<float>(i) / gridLines) * gridExtent;
            ImVec2 p0, p1;
            if (project3D(glm::vec3(coord, 0, -gridExtent), p0) && project3D(glm::vec3(coord, 0, gridExtent), p1)) {
                drawList->AddLine(p0, p1, (i == 0) ? IM_COL32(80, 120, 200, 180) : IM_COL32(35, 45, 60, 150), 1.0f);
            }
            if (project3D(glm::vec3(-gridExtent, 0, coord), p0) && project3D(glm::vec3(gridExtent, 0, coord), p1)) {
                drawList->AddLine(p0, p1, (i == 0) ? IM_COL32(200, 80, 80, 180) : IM_COL32(35, 45, 60, 150), 1.0f);
            }
        }
    }

    // Draw 3D Mesh Wireframe
    if (m_showWireframe) {
        const auto& verts = geom->GetVertices();
        const auto& indices = geom->GetIndices();

        if (!indices.empty()) {
            for (size_t i = 0; i + 2 < indices.size(); i += 3) {
                uint32_t i0 = indices[i];
                uint32_t i1 = indices[i + 1];
                uint32_t i2 = indices[i + 2];

                if (i0 < verts.size() && i1 < verts.size() && i2 < verts.size()) {
                    ImVec2 p0, p1, p2;
                    bool v0 = project3D(verts[i0].pos, p0);
                    bool v1 = project3D(verts[i1].pos, p1);
                    bool v2 = project3D(verts[i2].pos, p2);

                    if (v0 && v1) drawList->AddLine(p0, p1, IM_COL32(0, 220, 160, 200), 1.2f);
                    if (v1 && v2) drawList->AddLine(p1, p2, IM_COL32(0, 220, 160, 200), 1.2f);
                    if (v2 && v0) drawList->AddLine(p2, p0, IM_COL32(0, 220, 160, 200), 1.2f);
                }
            }
        } else {
            // Draw points / cloud
            for (const auto& v : verts) {
                ImVec2 p2;
                if (project3D(v.pos, p2)) {
                    drawList->AddCircleFilled(p2, 2.5f, IM_COL32(0, 255, 200, 240));
                }
            }
        }
    }

    // Overlay Camera Orbit Info
    ImGui::SetCursorScreenPos(ImVec2(canvasPos.x + 10.0f, canvasPos.y + canvasSize.y - 25.0f));
    ImGui::TextDisabled("Orbit: Yaw %.1f° | Pitch %.1f° | Dist %.2f", m_orbitYaw, m_orbitPitch, m_orbitDistance);
}

} // namespace nf::ui
