#pragma once

#include "../EditorContext.h"
#include "BreadcrumbBar.h"
#include <imgui.h>
#include <glm/vec2.hpp>
#include <vector>
#include <memory>

namespace nf::ui {

class NodeCanvas {
public:
    explicit NodeCanvas(EditorContext* ctx);
    ~NodeCanvas();

    void Render();

    // Coordinate Transforms
    ImVec2 CanvasToScreen(const glm::vec2& canvasPos) const;
    glm::vec2 ScreenToCanvas(const ImVec2& screenPos) const;

    // View Navigation
    void FrameSelectedNodes();
    void CenterView();

private:
    void DrawGrid(ImDrawList* drawList, const ImVec2& canvasStart, const ImVec2& canvasSize);
    void DrawWires(ImDrawList* drawList);
    void DrawNodes(ImDrawList* drawList);
    void DrawSelectionBox(ImDrawList* drawList);
    void DrawActiveDraggingWire(ImDrawList* drawList);
    void HandleInput();

    // Node Box Calculations
    ImVec2 GetNodeSize(Node* node) const;
    ImVec2 GetPinScreenPos(Node* node, Pin* pin, bool isInput) const;

    EditorContext* m_ctx;
    ImVec2 m_canvasOrigin{ 0.0f, 0.0f };
    ImVec2 m_canvasSize{ 0.0f, 0.0f };

    bool m_isPanning = false;
    bool m_isBoxSelecting = false;
    ImVec2 m_boxSelectStartScreen{ 0.0f, 0.0f };
    ImVec2 m_boxSelectEndScreen{ 0.0f, 0.0f };

    bool m_isDraggingNodes = false;
    std::unordered_map<NodeId, glm::vec2> m_dragStartPositions;

    std::unique_ptr<class BreadcrumbBar> m_breadcrumbBar;
};

} // namespace nf::ui
