#include "EditorContext.h"
#include "../operators/comp/ContainerComp.h"
#include "../project/ProjectSerializer.h"
#include "../project/ComponentSerializer.h"
#include "../project/RecentProjectsManager.h"
#include <spdlog/spdlog.h>

namespace nf::ui {

EditorContext::EditorContext() {
    m_navStack.push_back({ m_project.rootGraph.get(), nullptr, "/" });
    m_currentGraph = m_project.rootGraph.get();
}

void EditorContext::SetGraph(Graph* graph) {
    m_currentGraph = graph;
    ClearSelection();
}

Graph* EditorContext::GetGraph() const {
    if (m_currentGraph) return m_currentGraph;
    return m_project.rootGraph.get();
}

void EditorContext::EnterContainer(ContainerComp* container) {
    if (!container || !container->GetInnerGraph()) return;

    m_navStack.push_back({ container->GetInnerGraph(), container, container->GetName() });
    m_currentGraph = container->GetInnerGraph();
    ClearSelection();
}

void EditorContext::ExitContainer() {
    if (m_navStack.size() > 1) {
        m_navStack.pop_back();
        m_currentGraph = m_navStack.back().graph;
        ClearSelection();
    }
}

void EditorContext::NavigateToLevel(size_t levelIndex) {
    if (levelIndex < m_navStack.size()) {
        m_navStack.resize(levelIndex + 1);
        m_currentGraph = m_navStack.back().graph;
        ClearSelection();
    }
}

std::string EditorContext::GetCurrentPath() const {
    if (m_navStack.size() <= 1) return "/";

    std::string path;
    for (size_t i = 1; i < m_navStack.size(); ++i) {
        path += "/" + m_navStack[i].name;
    }
    return path;
}

void EditorContext::NewProject() {
    m_project.Reset();
    m_navStack.clear();
    m_navStack.push_back({ m_project.rootGraph.get(), nullptr, "/" });
    m_currentGraph = m_project.rootGraph.get();

    m_nodePositions.clear();
    m_selectedNodes.clear();
    m_activeNodeId = 0;
    m_undoManager.Clear();
    m_canvasPan = glm::vec2(0.0f, 0.0f);
    m_canvasZoom = 1.0f;
}

bool EditorContext::OpenProject(const std::string& filePath, std::string* outError) {
    if (filePath.empty()) return false;

    if (!project::ProjectSerializer::LoadFromFile(filePath, m_project, outError)) {
        return false;
    }

    m_navStack.clear();
    m_navStack.push_back({ m_project.rootGraph.get(), nullptr, "/" });
    m_currentGraph = m_project.rootGraph.get();

    m_nodePositions = m_project.nodePositions;
    m_canvasPan = m_project.viewport.pan;
    m_canvasZoom = m_project.viewport.zoom;
    m_selectedNodes.clear();
    m_activeNodeId = 0;
    m_undoManager.Clear();

    project::RecentProjectsManager::Instance().AddRecentProject(filePath);
    return true;
}

bool EditorContext::SaveProject(const std::string& filePath, std::string* outError) {
    std::string targetPath = filePath.empty() ? m_project.filePath : filePath;
    if (targetPath.empty()) {
        if (outError) *outError = "Cannot save: no file path specified";
        return false;
    }

    m_project.nodePositions = m_nodePositions;
    m_project.viewport.pan = m_canvasPan;
    m_project.viewport.zoom = m_canvasZoom;
    m_project.filePath = targetPath;

    if (project::ProjectSerializer::SaveToFile(targetPath, m_project, outError)) {
        m_project.isDirty = false;
        m_autosave.DiscardAutosave(targetPath);
        project::RecentProjectsManager::Instance().AddRecentProject(targetPath);
        return true;
    }

    return false;
}

bool EditorContext::ExportComponent(const std::string& filePath, ContainerComp* comp, std::string* outError) {
    if (!comp) {
        if (outError) *outError = "No component selected for export";
        return false;
    }
    return project::ComponentSerializer::ExportComponent(filePath, comp, outError);
}

ContainerComp* EditorContext::ImportComponent(const std::string& filePath, const glm::vec2& spawnPos, std::string* outError) {
    Graph* activeGraph = GetGraph();
    if (!activeGraph) return nullptr;

    ContainerComp* comp = project::ComponentSerializer::ImportComponent(filePath, activeGraph, spawnPos, outError);
    if (comp) {
        SetNodePosition(comp->GetId(), spawnPos);
        SelectNode(comp->GetId());
        MarkDirty(true);
    }
    return comp;
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
    Graph* g = GetGraph();
    if (!g || m_activeNodeId == 0) return nullptr;
    return g->GetNode(m_activeNodeId);
}

void EditorContext::SetNodePosition(NodeId id, const glm::vec2& pos) {
    m_nodePositions[id] = pos;
    MarkDirty(true);
}

glm::vec2 EditorContext::GetNodePosition(NodeId id) const {
    auto it = m_nodePositions.find(id);
    if (it != m_nodePositions.end()) {
        return it->second;
    }
    return glm::vec2(100.0f + static_cast<float>(id % 10) * 40.0f, 100.0f + static_cast<float>(id % 10) * 40.0f);
}

void EditorContext::OpenOpPalette(const glm::vec2& screenPos) {
    m_opPalettePos = screenPos;
    m_showOpPalette = true;
}

} // namespace nf::ui
