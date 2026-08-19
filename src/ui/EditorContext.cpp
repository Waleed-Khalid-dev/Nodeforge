#include "EditorContext.h"

namespace nf::ui {

EditorContext::EditorContext() {
}

void EditorContext::SetGraph(Graph* graph) {
    m_graph = graph;
    m_selectedNodes.clear();
    m_activeNodeId = 0;
    m_draggingFromPin = nullptr;
}

void EditorContext::SelectNode(NodeId id, bool addToSelection) {
    if (!addToSelection) {
        m_selectedNodes.clear();
    }
    m_selectedNodes.insert(id);
    m_activeNodeId = id;
}

void EditorContext::DeselectNode(NodeId id) {
    m_selectedNodes.erase(id);
    if (m_activeNodeId == id) {
        m_activeNodeId = m_selectedNodes.empty() ? 0 : *m_selectedNodes.begin();
    }
}

void EditorContext::ClearSelection() {
    m_selectedNodes.clear();
    m_activeNodeId = 0;
}

bool EditorContext::IsNodeSelected(NodeId id) const {
    return m_selectedNodes.find(id) != m_selectedNodes.end();
}

Node* EditorContext::GetActiveNode() const {
    if (!m_graph || m_activeNodeId == 0) return nullptr;
    return m_graph->GetNode(m_activeNodeId);
}

void EditorContext::SetNodePosition(NodeId id, const glm::vec2& pos) {
    m_nodePositions[id] = pos;
}

glm::vec2 EditorContext::GetNodePosition(NodeId id) const {
    auto it = m_nodePositions.find(id);
    if (it != m_nodePositions.end()) {
        return it->second;
    }
    // Default staggered layout for newly created nodes
    return glm::vec2(100.0f + static_cast<float>(id % 10) * 40.0f, 100.0f + static_cast<float>(id % 10) * 40.0f);
}

void EditorContext::OpenOpPalette(const glm::vec2& screenPos) {
    m_showOpPalette = true;
    m_opPalettePos = screenPos;
}

} // namespace nf::ui
