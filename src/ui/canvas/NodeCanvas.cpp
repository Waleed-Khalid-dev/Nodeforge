#include "NodeCanvas.h"
#include "BreadcrumbBar.h"
#include "../EditorTheme.h"
#include "../commands/NodeCommands.h"
#include "../commands/WireCommands.h"
#include "../../operators/tex/TexOp.h"
#include "../../operators/comp/ContainerComp.h"
#include "../../core/ChannelBuffer.h"
#include "../../profiling/CookProfiler.h"
#include <algorithm>
#include <cmath>

namespace nf::ui {

NodeCanvas::NodeCanvas(EditorContext* ctx)
    : m_ctx(ctx), m_breadcrumbBar(std::make_unique<BreadcrumbBar>(ctx)) {
}

NodeCanvas::~NodeCanvas() = default;

ImVec2 NodeCanvas::CanvasToScreen(const glm::vec2& canvasPos) const {
    float zoom = m_ctx->GetCanvasZoom();
    const glm::vec2& pan = m_ctx->GetCanvasPan();
    return ImVec2(
        m_canvasOrigin.x + (canvasPos.x + pan.x) * zoom,
        m_canvasOrigin.y + (canvasPos.y + pan.y) * zoom
    );
}

glm::vec2 NodeCanvas::ScreenToCanvas(const ImVec2& screenPos) const {
    float zoom = m_ctx->GetCanvasZoom();
    const glm::vec2& pan = m_ctx->GetCanvasPan();
    if (zoom <= 0.001f) zoom = 1.0f;
    return glm::vec2(
        (screenPos.x - m_canvasOrigin.x) / zoom - pan.x,
        (screenPos.y - m_canvasOrigin.y) / zoom - pan.y
    );
}

ImVec2 NodeCanvas::GetNodeSize(Node* node) const {
    if (!node) return ImVec2(180.0f, 80.0f);
    float zoom = m_ctx->GetCanvasZoom();
    float width = 190.0f * zoom;
    size_t pinCount = std::max(node->GetInputPins().size(), node->GetOutputPins().size());
    float baseHeight = 50.0f + static_cast<float>(pinCount) * 22.0f;

    // Additional height if node has preview thumbnail or waveform
    if (node->GetFamily() == NodeFamily::TexOp || node->GetFamily() == NodeFamily::ChanOp) {
        baseHeight += 40.0f;
    }
    return ImVec2(width, baseHeight * zoom);
}

ImVec2 NodeCanvas::GetPinScreenPos(Node* node, Pin* pin, bool isInput) const {
    if (!node || !pin) return ImVec2(0.0f, 0.0f);
    glm::vec2 nodeCanvasPos = m_ctx->GetNodePosition(node->GetId());
    ImVec2 nodeScreenPos = CanvasToScreen(nodeCanvasPos);
    ImVec2 nodeSize = GetNodeSize(node);
    float zoom = m_ctx->GetCanvasZoom();

    const auto& pins = isInput ? node->GetInputPins() : node->GetOutputPins();
    int pinIndex = 0;
    for (size_t i = 0; i < pins.size(); ++i) {
        if (pins[i].get() == pin) {
            pinIndex = static_cast<int>(i);
            break;
        }
    }

    float headerH = 28.0f * zoom;
    float yOffset = headerH + (16.0f + static_cast<float>(pinIndex) * 22.0f) * zoom;
    float x = isInput ? nodeScreenPos.x : (nodeScreenPos.x + nodeSize.x);
    return ImVec2(x, nodeScreenPos.y + yOffset);
}

void NodeCanvas::Render() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Node Graph", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    if (m_breadcrumbBar) {
        m_breadcrumbBar->Render();
    }

    m_canvasOrigin = ImGui::GetCursorScreenPos();
    m_canvasSize = ImGui::GetContentRegionAvail();
    if (m_canvasSize.x < 50.0f) m_canvasSize.x = 50.0f;
    if (m_canvasSize.y < 50.0f) m_canvasSize.y = 50.0f;

    ImDrawList* drawList = ImGui::GetWindowDrawList();

    // Canvas background
    drawList->AddRectFilled(m_canvasOrigin, ImVec2(m_canvasOrigin.x + m_canvasSize.x, m_canvasOrigin.y + m_canvasSize.y), EditorTheme::ColBgDark);

    // Draw grid
    DrawGrid(drawList, m_canvasOrigin, m_canvasSize);

    // Handle user inputs (pan, zoom, select, drag)
    HandleInput();

    // Draw wires
    DrawWires(drawList);

    // Draw active dragging wire
    DrawActiveDraggingWire(drawList);

    // Draw nodes
    DrawNodes(drawList);

    // Draw selection box
    DrawSelectionBox(drawList);

    ImGui::End();
    ImGui::PopStyleVar();
}

void NodeCanvas::DrawGrid(ImDrawList* drawList, const ImVec2& canvasStart, const ImVec2& canvasSize) {
    float zoom = m_ctx->GetCanvasZoom();
    const glm::vec2& pan = m_ctx->GetCanvasPan();

    float minorStep = 24.0f * zoom;
    float majorStep = 120.0f * zoom;

    float startX = std::fmod(pan.x * zoom, minorStep);
    float startY = std::fmod(pan.y * zoom, minorStep);

    // Minor grid lines
    if (zoom > 0.4f) {
        for (float x = startX; x < canvasSize.x; x += minorStep) {
            drawList->AddLine(ImVec2(canvasStart.x + x, canvasStart.y), ImVec2(canvasStart.x + x, canvasStart.y + canvasSize.y), EditorTheme::ColGridMinor);
        }
        for (float y = startY; y < canvasSize.y; y += minorStep) {
            drawList->AddLine(ImVec2(canvasStart.x, canvasStart.y + y), ImVec2(canvasStart.x + canvasSize.x, canvasStart.y + y), EditorTheme::ColGridMinor);
        }
    }

    // Major grid lines
    float majorStartX = std::fmod(pan.x * zoom, majorStep);
    float majorStartY = std::fmod(pan.y * zoom, majorStep);
    for (float x = majorStartX; x < canvasSize.x; x += majorStep) {
        drawList->AddLine(ImVec2(canvasStart.x + x, canvasStart.y), ImVec2(canvasStart.x + x, canvasStart.y + canvasSize.y), EditorTheme::ColGridMajor, 1.5f);
    }
    for (float y = majorStartY; y < canvasSize.y; y += majorStep) {
        drawList->AddLine(ImVec2(canvasStart.x, canvasStart.y + y), ImVec2(canvasStart.x + canvasSize.x, canvasStart.y + y), EditorTheme::ColGridMajor, 1.5f);
    }
}

void NodeCanvas::DrawWires(ImDrawList* drawList) {
    if (!m_ctx || !m_ctx->GetGraph()) return;
    float zoom = m_ctx->GetCanvasZoom();

    for (const auto& [wireId, wire] : m_ctx->GetGraph()->GetWires()) {
        Pin* fromPin = wire->GetFromPin();
        Pin* toPin = wire->GetToPin();
        if (!fromPin || !toPin) continue;

        ImVec2 p1 = GetPinScreenPos(fromPin->GetNode(), fromPin, false);
        ImVec2 p2 = GetPinScreenPos(toPin->GetNode(), toPin, true);

        float dx = std::abs(p2.x - p1.x) * 0.5f;
        if (dx < 40.0f * zoom) dx = 40.0f * zoom;

        ImVec2 cp1(p1.x + dx, p1.y);
        ImVec2 cp2(p2.x - dx, p2.y);

        ImU32 wireColor = EditorTheme::GetPinColorU32(fromPin->GetType());
        drawList->AddBezierCubic(p1, cp1, cp2, p2, wireColor, 2.5f * zoom);
    }
}

void NodeCanvas::DrawActiveDraggingWire(ImDrawList* drawList) {
    Pin* draggingPin = m_ctx->GetDraggingWirePin();
    if (!draggingPin) return;

    float zoom = m_ctx->GetCanvasZoom();
    bool isInput = (draggingPin->GetDirection() == PinDirection::Input);
    ImVec2 p1 = GetPinScreenPos(draggingPin->GetNode(), draggingPin, isInput);
    ImVec2 p2 = ImGui::GetMousePos();

    float dx = std::abs(p2.x - p1.x) * 0.5f;
    ImVec2 cp1 = isInput ? ImVec2(p1.x - dx, p1.y) : ImVec2(p1.x + dx, p1.y);
    ImVec2 cp2 = isInput ? ImVec2(p2.x + dx, p2.y) : ImVec2(p2.x - dx, p2.y);

    ImU32 wireColor = EditorTheme::GetPinColorU32(draggingPin->GetType());
    drawList->AddBezierCubic(p1, cp1, cp2, p2, wireColor, 2.5f * zoom);
}

void NodeCanvas::DrawNodes(ImDrawList* drawList) {
    if (!m_ctx || !m_ctx->GetGraph()) return;
    float zoom = m_ctx->GetCanvasZoom();

    for (const auto& [nodeId, node] : m_ctx->GetGraph()->GetNodes()) {
        glm::vec2 canvasPos = m_ctx->GetNodePosition(nodeId);
        ImVec2 screenPos = CanvasToScreen(canvasPos);
        ImVec2 nodeSize = GetNodeSize(node.get());
        ImVec2 nodeMax(screenPos.x + nodeSize.x, screenPos.y + nodeSize.y);

        bool isSelected = m_ctx->IsNodeSelected(nodeId);
        ImU32 headerCol = EditorTheme::GetFamilyColorU32(node->GetFamily());

        // Node Box Shadow & Background
        drawList->AddRectFilled(screenPos, nodeMax, isSelected ? EditorTheme::ColNodeBgSelected : EditorTheme::ColNodeBg, 6.0f * zoom);

        // Header Background
        float headerH = 26.0f * zoom;
        ImVec2 headerMax(nodeMax.x, screenPos.y + headerH);
        drawList->AddRectFilled(screenPos, headerMax, headerCol, 6.0f * zoom, ImDrawFlags_RoundCornersTop);

        // Selection / Outline Glow
        ImU32 borderCol = isSelected ? EditorTheme::ColBorderActive : EditorTheme::ColBorder;
        drawList->AddRect(screenPos, nodeMax, borderCol, 6.0f * zoom, 0, isSelected ? (2.0f * zoom) : (1.0f * zoom));

        // Node Header Text & Live Profiler Badge
        if (zoom > 0.4f) {
            ImVec2 textPos(screenPos.x + 8.0f * zoom, screenPos.y + 4.0f * zoom);
            drawList->AddText(textPos, IM_COL32(255, 255, 255, 255), node->GetName().c_str());

            NodePerfStats stats;
            if (CookProfiler::Instance().GetNodeStats(nodeId, stats)) {
                char badgeStr[32];
                snprintf(badgeStr, sizeof(badgeStr), "%.2fms", stats.lastCpuMs);
                ImVec2 badgeSize = ImGui::CalcTextSize(badgeStr);
                ImVec2 badgePos(nodeMax.x - badgeSize.x - 8.0f * zoom, screenPos.y + 4.0f * zoom);
                ImU32 badgeCol = (stats.lastCpuMs > 4.0) ? IM_COL32(255, 80, 80, 240) :
                                 (stats.lastCpuMs > 1.0) ? IM_COL32(255, 200, 50, 240) :
                                                           IM_COL32(80, 230, 120, 240);
                drawList->AddText(badgePos, badgeCol, badgeStr);
            }
        }

        // Draw Pins
        // Input Pins (Left)
        for (const auto& pin : node->GetInputPins()) {
            ImVec2 pinPos = GetPinScreenPos(node.get(), pin.get(), true);
            ImU32 pinCol = EditorTheme::GetPinColorU32(pin->GetType());
            float radius = 4.5f * zoom;

            if (pin->IsConnected()) {
                drawList->AddCircleFilled(pinPos, radius, pinCol);
            } else {
                drawList->AddCircle(pinPos, radius, pinCol, 12, 1.5f * zoom);
            }

            if (zoom > 0.5f) {
                drawList->AddText(ImVec2(pinPos.x + 8.0f * zoom, pinPos.y - 7.0f * zoom), IM_COL32(200, 205, 215, 230), pin->GetName().c_str());
            }
        }

        // Output Pins (Right)
        for (const auto& pin : node->GetOutputPins()) {
            ImVec2 pinPos = GetPinScreenPos(node.get(), pin.get(), false);
            ImU32 pinCol = EditorTheme::GetPinColorU32(pin->GetType());
            float radius = 4.5f * zoom;

            if (pin->IsConnected()) {
                drawList->AddCircleFilled(pinPos, radius, pinCol);
            } else {
                drawList->AddCircle(pinPos, radius, pinCol, 12, 1.5f * zoom);
            }

            if (zoom > 0.5f) {
                ImVec2 textSize = ImGui::CalcTextSize(pin->GetName().c_str());
                drawList->AddText(ImVec2(pinPos.x - 8.0f * zoom - textSize.x, pinPos.y - 7.0f * zoom), IM_COL32(200, 205, 215, 230), pin->GetName().c_str());
            }
        }

        // Mini waveform preview for ChanOp nodes
        if (zoom > 0.5f && node->GetFamily() == NodeFamily::ChanOp) {
            for (const auto& pin : node->GetOutputPins()) {
                if (pin->GetValue().Is<ChannelBuffer>()) {
                    const auto& buf = pin->GetValue().Get<ChannelBuffer>();
                    if (!buf.IsEmpty()) {
                        float previewX = screenPos.x + 12.0f * zoom;
                        float previewW = nodeSize.x - 24.0f * zoom;
                        float previewH = 28.0f * zoom;
                        float previewY = nodeMax.y - previewH - 8.0f * zoom;

                        if (previewH > 10.0f * zoom && previewW > 20.0f * zoom) {
                            drawList->AddRectFilled(ImVec2(previewX, previewY), ImVec2(previewX + previewW, previewY + previewH), IM_COL32(10, 12, 16, 200), 3.0f * zoom);
                            drawList->AddRect(ImVec2(previewX, previewY), ImVec2(previewX + previewW, previewY + previewH), IM_COL32(40, 50, 65, 200), 3.0f * zoom);

                            const float* d = buf.GetChannelData(0);
                            size_t samples = buf.GetSampleCount();
                            if (d && samples > 1) {
                                float dx = previewW / static_cast<float>(samples - 1);
                                for (size_t s = 0; s < samples - 1; ++s) {
                                    float x0 = previewX + static_cast<float>(s) * dx;
                                    float y0 = previewY + previewH * (1.0f - std::clamp((d[s] + 1.0f) * 0.5f, 0.0f, 1.0f));
                                    float x1 = previewX + static_cast<float>(s + 1) * dx;
                                    float y1 = previewY + previewH * (1.0f - std::clamp((d[s + 1] + 1.0f) * 0.5f, 0.0f, 1.0f));
                                    drawList->AddLine(ImVec2(x0, y0), ImVec2(x1, y1), IM_COL32(0, 220, 255, 230), 1.5f * zoom);
                                }
                            }
                        }
                    }
                    break;
                }
            }
        }

        // Mini table preview for DataOp nodes
        if (zoom > 0.5f && node->GetFamily() == NodeFamily::DataOp) {
            for (const auto& pin : node->GetOutputPins()) {
                if (pin->GetValue().Is<DataTable>()) {
                    const auto& dt = pin->GetValue().Get<DataTable>();
                    if (!dt.IsEmpty()) {
                        float previewX = screenPos.x + 12.0f * zoom;
                        float previewW = nodeSize.x - 24.0f * zoom;
                        float previewH = 28.0f * zoom;
                        float previewY = nodeMax.y - previewH - 8.0f * zoom;

                        if (previewH > 10.0f * zoom && previewW > 20.0f * zoom) {
                            drawList->AddRectFilled(ImVec2(previewX, previewY), ImVec2(previewX + previewW, previewY + previewH), IM_COL32(18, 14, 10, 220), 3.0f * zoom);
                            drawList->AddRect(ImVec2(previewX, previewY), ImVec2(previewX + previewW, previewY + previewH), IM_COL32(75, 45, 20, 200), 3.0f * zoom);

                            // Draw mini grid lines
                            float midX = previewX + previewW * 0.5f;
                            float midY = previewY + previewH * 0.5f;
                            drawList->AddLine(ImVec2(midX, previewY), ImVec2(midX, previewY + previewH), IM_COL32(90, 55, 30, 150), 1.0f * zoom);
                            drawList->AddLine(ImVec2(previewX, midY), ImVec2(previewX + previewW, midY), IM_COL32(90, 55, 30, 150), 1.0f * zoom);

                            // Draw table dimension label
                            std::string snippet = std::to_string(dt.GetRowCount()) + "x" + std::to_string(dt.GetColumnCount());
                            drawList->AddText(ImVec2(previewX + 4.0f * zoom, previewY + 2.0f * zoom), IM_COL32(255, 170, 80, 220), snippet.c_str());
                        }
                    }
                    break;
                }
            }
        }

        // Mini isometric 3D wireframe preview for GeomOp nodes
        if (zoom > 0.5f && node->GetFamily() == NodeFamily::GeomOp) {
            for (const auto& pin : node->GetOutputPins()) {
                if (pin->GetValue().Is<GeometryData>()) {
                    const auto& geom = pin->GetValue().Get<GeometryData>();
                    if (!geom.IsEmpty()) {
                        float previewX = screenPos.x + 12.0f * zoom;
                        float previewW = nodeSize.x - 24.0f * zoom;
                        float previewH = 28.0f * zoom;
                        float previewY = nodeMax.y - previewH - 8.0f * zoom;

                        if (previewH > 10.0f * zoom && previewW > 20.0f * zoom) {
                            drawList->AddRectFilled(ImVec2(previewX, previewY), ImVec2(previewX + previewW, previewY + previewH), IM_COL32(10, 20, 16, 220), 3.0f * zoom);
                            drawList->AddRect(ImVec2(previewX, previewY), ImVec2(previewX + previewW, previewY + previewH), IM_COL32(30, 80, 60, 200), 3.0f * zoom);

                            // Draw mini isometric cube wireframe representation
                            float cx = previewX + previewW * 0.5f;
                            float cy = previewY + previewH * 0.5f;
                            float r = 8.0f * zoom;

                            ImVec2 pT(cx, cy - r);
                            ImVec2 pB(cx, cy + r);
                            ImVec2 pL(cx - r * 0.866f, cy - r * 0.5f);
                            ImVec2 pR(cx + r * 0.866f, cy - r * 0.5f);
                            ImVec2 pBL(cx - r * 0.866f, cy + r * 0.5f);
                            ImVec2 pBR(cx + r * 0.866f, cy + r * 0.5f);
                            ImVec2 pC(cx, cy);

                            drawList->AddLine(pT, pR, IM_COL32(0, 255, 180, 220), 1.0f * zoom);
                            drawList->AddLine(pR, pBR, IM_COL32(0, 255, 180, 220), 1.0f * zoom);
                            drawList->AddLine(pBR, pB, IM_COL32(0, 255, 180, 220), 1.0f * zoom);
                            drawList->AddLine(pB, pBL, IM_COL32(0, 255, 180, 220), 1.0f * zoom);
                            drawList->AddLine(pBL, pL, IM_COL32(0, 255, 180, 220), 1.0f * zoom);
                            drawList->AddLine(pL, pT, IM_COL32(0, 255, 180, 220), 1.0f * zoom);
                            drawList->AddLine(pC, pT, IM_COL32(0, 200, 140, 180), 1.0f * zoom);
                            drawList->AddLine(pC, pBL, IM_COL32(0, 200, 140, 180), 1.0f * zoom);
                            drawList->AddLine(pC, pBR, IM_COL32(0, 200, 140, 180), 1.0f * zoom);
                        }
                    }
                    break;
                }
            }
        }
    }
}

void NodeCanvas::DrawSelectionBox(ImDrawList* drawList) {
    if (!m_isBoxSelecting) return;
    ImVec2 pMin(
        std::min(m_boxSelectStartScreen.x, m_boxSelectEndScreen.x),
        std::min(m_boxSelectStartScreen.y, m_boxSelectEndScreen.y)
    );
    ImVec2 pMax(
        std::max(m_boxSelectStartScreen.x, m_boxSelectEndScreen.x),
        std::max(m_boxSelectStartScreen.y, m_boxSelectEndScreen.y)
    );
    drawList->AddRectFilled(pMin, pMax, IM_COL32(70, 130, 240, 40));
    drawList->AddRect(pMin, pMax, IM_COL32(90, 150, 255, 200), 0.0f, 0, 1.5f);
}

void NodeCanvas::HandleInput() {
    ImGuiIO& io = ImGui::GetIO();
    bool isHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
    ImVec2 mousePos = ImGui::GetMousePos();

    // 1. Zooming with Mouse Wheel
    if (isHovered && io.MouseWheel != 0.0f) {
        float oldZoom = m_ctx->GetCanvasZoom();
        float newZoom = std::clamp(oldZoom + io.MouseWheel * 0.1f * oldZoom, 0.2f, 2.5f);
        if (newZoom != oldZoom) {
            glm::vec2 mouseCanvasBefore = ScreenToCanvas(mousePos);
            m_ctx->GetCanvasZoom() = newZoom;
            glm::vec2 mouseCanvasAfter = ScreenToCanvas(mousePos);
            m_ctx->GetCanvasPan() += (mouseCanvasAfter - mouseCanvasBefore);
        }
    }

    // 2. Panning with Middle-drag or Right-drag
    if (isHovered && (ImGui::IsMouseDragging(ImGuiMouseButton_Middle) || (ImGui::IsMouseDragging(ImGuiMouseButton_Right) && !m_ctx->GetDraggingWirePin()))) {
        m_ctx->GetCanvasPan().x += io.MouseDelta.x / m_ctx->GetCanvasZoom();
        m_ctx->GetCanvasPan().y += io.MouseDelta.y / m_ctx->GetCanvasZoom();
    }

    // 3. Node & Pin Clicking / Dragging
    if (isHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        bool clickedNodeOrPin = false;

        if (m_ctx->GetGraph()) {
            for (const auto& [nodeId, node] : m_ctx->GetGraph()->GetNodes()) {
                // Check Pin clicks
                for (const auto& pin : node->GetOutputPins()) {
                    ImVec2 pinPos = GetPinScreenPos(node.get(), pin.get(), false);
                    float dist = std::hypot(mousePos.x - pinPos.x, mousePos.y - pinPos.y);
                    if (dist <= 12.0f * m_ctx->GetCanvasZoom()) {
                        m_ctx->StartDraggingWire(pin.get());
                        clickedNodeOrPin = true;
                        break;
                    }
                }
                if (clickedNodeOrPin) break;

                for (const auto& pin : node->GetInputPins()) {
                    ImVec2 pinPos = GetPinScreenPos(node.get(), pin.get(), true);
                    float dist = std::hypot(mousePos.x - pinPos.x, mousePos.y - pinPos.y);
                    if (dist <= 12.0f * m_ctx->GetCanvasZoom()) {
                        if (pin->IsConnected()) {
                            // Dragging an existing connection off
                            Pin* fromPin = pin->GetConnectedSource();
                            m_ctx->GetUndoManager().ExecuteCommand(std::make_unique<DisconnectWireCommand>(m_ctx, pin.get()));
                            if (fromPin) {
                                m_ctx->StartDraggingWire(fromPin);
                            }
                        } else {
                            m_ctx->StartDraggingWire(pin.get());
                        }
                        clickedNodeOrPin = true;
                        break;
                    }
                }
                if (clickedNodeOrPin) break;

                // Check Node body click
                glm::vec2 canvasPos = m_ctx->GetNodePosition(nodeId);
                ImVec2 screenPos = CanvasToScreen(canvasPos);
                ImVec2 nodeSize = GetNodeSize(node.get());

                if (mousePos.x >= screenPos.x && mousePos.x <= screenPos.x + nodeSize.x &&
                    mousePos.y >= screenPos.y && mousePos.y <= screenPos.y + nodeSize.y) {
                    
                    if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                        if (auto* comp = dynamic_cast<ContainerComp*>(node.get())) {
                            m_ctx->EnterContainer(comp);
                            return;
                        }
                    }

                    bool multi = io.KeyCtrl || io.KeyShift;
                    if (!m_ctx->IsNodeSelected(nodeId)) {
                        m_ctx->SelectNode(nodeId, multi);
                    }
                    clickedNodeOrPin = true;
                    m_isDraggingNodes = true;
                    m_dragStartPositions.clear();
                    for (NodeId selId : m_ctx->GetSelectedNodes()) {
                        m_dragStartPositions[selId] = m_ctx->GetNodePosition(selId);
                    }
                    break;
                }
            }
        }

        // Empty canvas click -> Box selection or Deselect
        if (!clickedNodeOrPin) {
            if (!io.KeyCtrl && !io.KeyShift) {
                m_ctx->ClearSelection();
            }
            m_isBoxSelecting = true;
            m_boxSelectStartScreen = mousePos;
            m_boxSelectEndScreen = mousePos;
        }
    }

    // 4. Update Node Dragging
    if (m_isDraggingNodes && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        float zoom = m_ctx->GetCanvasZoom();
        glm::vec2 deltaCanvas(io.MouseDelta.x / zoom, io.MouseDelta.y / zoom);
        for (NodeId selId : m_ctx->GetSelectedNodes()) {
            glm::vec2 cur = m_ctx->GetNodePosition(selId);
            m_ctx->SetNodePosition(selId, cur + deltaCanvas);
        }
    }

    // Node Drag Release -> Record Undo
    if (m_isDraggingNodes && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
        m_isDraggingNodes = false;
        std::unordered_map<NodeId, glm::vec2> finalPositions;
        for (NodeId selId : m_ctx->GetSelectedNodes()) {
            finalPositions[selId] = m_ctx->GetNodePosition(selId);
        }
        m_ctx->GetUndoManager().ExecuteCommand(std::make_unique<MoveNodesCommand>(m_ctx, m_dragStartPositions, finalPositions));
    }

    // 5. Update Box Selection
    if (m_isBoxSelecting) {
        m_boxSelectEndScreen = mousePos;
        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
            m_isBoxSelecting = false;
            // Compute selected nodes
            glm::vec2 bMin = ScreenToCanvas(ImVec2(std::min(m_boxSelectStartScreen.x, m_boxSelectEndScreen.x), std::min(m_boxSelectStartScreen.y, m_boxSelectEndScreen.y)));
            glm::vec2 bMax = ScreenToCanvas(ImVec2(std::max(m_boxSelectStartScreen.x, m_boxSelectEndScreen.x), std::max(m_boxSelectStartScreen.y, m_boxSelectEndScreen.y)));

            if (m_ctx->GetGraph()) {
                for (const auto& [nodeId, node] : m_ctx->GetGraph()->GetNodes()) {
                    glm::vec2 nPos = m_ctx->GetNodePosition(nodeId);
                    if (nPos.x >= bMin.x && nPos.x <= bMax.x && nPos.y >= bMin.y && nPos.y <= bMax.y) {
                        m_ctx->SelectNode(nodeId, true);
                    }
                }
            }
        }
    }

    // 6. Wire Release -> Connect or Discard
    if (m_ctx->GetDraggingWirePin() && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
        Pin* fromPin = m_ctx->GetDraggingWirePin();
        Pin* targetPin = nullptr;

        if (m_ctx->GetGraph()) {
            for (const auto& [nodeId, node] : m_ctx->GetGraph()->GetNodes()) {
                const auto& candidatePins = (fromPin->GetDirection() == PinDirection::Output) ? node->GetInputPins() : node->GetOutputPins();
                for (const auto& pin : candidatePins) {
                    ImVec2 pPos = GetPinScreenPos(node.get(), pin.get(), pin->GetDirection() == PinDirection::Input);
                    if (std::hypot(mousePos.x - pPos.x, mousePos.y - pPos.y) <= 14.0f * m_ctx->GetCanvasZoom()) {
                        targetPin = pin.get();
                        break;
                    }
                }
                if (targetPin) break;
            }
        }

        if (targetPin && targetPin->GetNode() != fromPin->GetNode()) {
            Pin* outPin = (fromPin->GetDirection() == PinDirection::Output) ? fromPin : targetPin;
            Pin* inPin = (fromPin->GetDirection() == PinDirection::Input) ? fromPin : targetPin;
            m_ctx->GetUndoManager().ExecuteCommand(std::make_unique<ConnectWireCommand>(m_ctx, outPin, inPin));
        }
        m_ctx->StopDraggingWire();
    }

    // 7. Global Canvas Hotkeys
    if (isHovered) {
        if (ImGui::IsKeyPressed(ImGuiKey_Tab)) {
            m_ctx->OpenOpPalette(glm::vec2(mousePos.x, mousePos.y));
        }
        if (ImGui::IsKeyPressed(ImGuiKey_U) || (ImGui::IsKeyPressed(ImGuiKey_Backspace) && m_ctx->GetSelectedNodes().empty())) {
            m_ctx->ExitContainer();
        }
        if (ImGui::IsKeyPressed(ImGuiKey_Delete) || (ImGui::IsKeyPressed(ImGuiKey_Backspace) && !m_ctx->GetSelectedNodes().empty())) {
            for (NodeId selId : m_ctx->GetSelectedNodes()) {
                m_ctx->GetUndoManager().ExecuteCommand(std::make_unique<DeleteNodeCommand>(m_ctx, selId));
            }
        }
        if (ImGui::IsKeyPressed(ImGuiKey_F)) {
            FrameSelectedNodes();
        }
    }
}

void NodeCanvas::FrameSelectedNodes() {
    if (!m_ctx || !m_ctx->GetGraph() || m_ctx->GetSelectedNodes().empty()) {
        CenterView();
        return;
    }

    glm::vec2 minPos(1e9f, 1e9f);
    glm::vec2 maxPos(-1e9f, -1e9f);

    for (NodeId id : m_ctx->GetSelectedNodes()) {
        glm::vec2 p = m_ctx->GetNodePosition(id);
        minPos = glm::min(minPos, p);
        maxPos = glm::max(maxPos, p + glm::vec2(180.0f, 80.0f));
    }

    glm::vec2 center = (minPos + maxPos) * 0.5f;
    m_ctx->GetCanvasPan() = -center + glm::vec2(m_canvasSize.x * 0.5f / m_ctx->GetCanvasZoom(), m_canvasSize.y * 0.5f / m_ctx->GetCanvasZoom());
}

void NodeCanvas::CenterView() {
    m_ctx->GetCanvasPan() = glm::vec2(m_canvasSize.x * 0.5f, m_canvasSize.y * 0.5f);
}

} // namespace nf::ui
