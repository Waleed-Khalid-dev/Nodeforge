#pragma once

#include "../graph/Graph.h"
#include "UndoManager.h"
#include <glm/vec2.hpp>
#include <unordered_set>
#include <unordered_map>
#include <string>

namespace nf::ui {

class EditorContext {
public:
    EditorContext();

    void SetGraph(Graph* graph);
    Graph* GetGraph() const { return m_graph; }

    UndoManager& GetUndoManager() { return m_undoManager; }

    // Selection management
    void SelectNode(NodeId id, bool addToSelection = false);
    void DeselectNode(NodeId id);
    void ClearSelection();
    bool IsNodeSelected(NodeId id) const;
    const std::unordered_set<NodeId>& GetSelectedNodes() const { return m_selectedNodes; }
    Node* GetActiveNode() const;
    NodeId GetActiveNodeId() const { return m_activeNodeId; }

    // Node Positions (Canvas space)
    void SetNodePosition(NodeId id, const glm::vec2& pos);
    glm::vec2 GetNodePosition(NodeId id) const;

    // View Navigation
    glm::vec2& GetCanvasPan() { return m_canvasPan; }
    const glm::vec2& GetCanvasPan() const { return m_canvasPan; }
    float& GetCanvasZoom() { return m_canvasZoom; }
    float GetCanvasZoom() const { return m_canvasZoom; }

    // Interactive Wiring
    void StartDraggingWire(Pin* fromPin) { m_draggingFromPin = fromPin; }
    Pin* GetDraggingWirePin() const { return m_draggingFromPin; }
    void StopDraggingWire() { m_draggingFromPin = nullptr; }

    // OP Palette Dialog
    bool IsOpPaletteOpen() const { return m_showOpPalette; }
    void OpenOpPalette(const glm::vec2& screenPos);
    void CloseOpPalette() { m_showOpPalette = false; }
    const glm::vec2& GetOpPalettePos() const { return m_opPalettePos; }

    // Playback state
    bool IsPlaying() const { return m_isPlaying; }
    void SetPlaying(bool playing) { m_isPlaying = playing; }
    uint64_t GetCurrentFrame() const { return m_currentFrame; }
    void SetCurrentFrame(uint64_t frame) { m_currentFrame = frame; }
    float GetPlaybackFps() const { return m_playbackFps; }

private:
    Graph* m_graph = nullptr;
    UndoManager m_undoManager;

    std::unordered_set<NodeId> m_selectedNodes;
    NodeId m_activeNodeId = 0;
    std::unordered_map<NodeId, glm::vec2> m_nodePositions;

    glm::vec2 m_canvasPan{ 0.0f, 0.0f };
    float m_canvasZoom = 1.0f;

    Pin* m_draggingFromPin = nullptr;

    bool m_showOpPalette = false;
    glm::vec2 m_opPalettePos{ 0.0f, 0.0f };

    bool m_isPlaying = true;
    uint64_t m_currentFrame = 1;
    float m_playbackFps = 60.0f;
};

} // namespace nf::ui
