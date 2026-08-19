#include "NodeCommands.h"
#include "../EditorContext.h"

namespace nf::ui {

CreateNodeCommand::CreateNodeCommand(EditorContext* ctx, const std::string& typeName, const std::string& name, const glm::vec2& pos)
    : m_ctx(ctx), m_typeName(typeName), m_name(name), m_position(pos) {
}

void CreateNodeCommand::Execute() {
    if (!m_ctx || !m_ctx->GetGraph()) return;
    Graph* g = m_ctx->GetGraph();

    if (m_createdNodeId == 0) {
        m_createdNodeId = g->GenerateNodeId();
    }

    auto node = NodeRegistry::Instance().CreateNode(m_typeName, m_createdNodeId, m_name);
    if (node) {
        g->AddNode(std::move(node));
        m_ctx->SetNodePosition(m_createdNodeId, m_position);
        m_ctx->SelectNode(m_createdNodeId);
    }
}

void CreateNodeCommand::Undo() {
    if (!m_ctx || !m_ctx->GetGraph() || m_createdNodeId == 0) return;
    m_ctx->GetGraph()->RemoveNode(m_createdNodeId);
    m_ctx->DeselectNode(m_createdNodeId);
}

DeleteNodeCommand::DeleteNodeCommand(EditorContext* ctx, NodeId nodeId)
    : m_ctx(ctx), m_nodeId(nodeId) {
    if (m_ctx && m_ctx->GetGraph()) {
        Node* n = m_ctx->GetGraph()->GetNode(nodeId);
        if (n) {
            m_typeName = n->GetTypeName();
            m_nodeName = n->GetName();
            m_position = m_ctx->GetNodePosition(nodeId);
        }
    }
}

void DeleteNodeCommand::Execute() {
    if (!m_ctx || !m_ctx->GetGraph()) return;
    m_ctx->GetGraph()->RemoveNode(m_nodeId);
    m_ctx->DeselectNode(m_nodeId);
}

void DeleteNodeCommand::Undo() {
    if (!m_ctx || !m_ctx->GetGraph() || m_typeName.empty()) return;
    auto node = NodeRegistry::Instance().CreateNode(m_typeName, m_nodeId, m_nodeName);
    if (node) {
        m_ctx->GetGraph()->AddNode(std::move(node));
        m_ctx->SetNodePosition(m_nodeId, m_position);
        m_ctx->SelectNode(m_nodeId);
    }
}

MoveNodesCommand::MoveNodesCommand(EditorContext* ctx, const std::unordered_map<NodeId, glm::vec2>& oldPositions, const std::unordered_map<NodeId, glm::vec2>& newPositions)
    : m_ctx(ctx), m_oldPositions(oldPositions), m_newPositions(newPositions) {
}

void MoveNodesCommand::Execute() {
    if (!m_ctx) return;
    for (const auto& [id, pos] : m_newPositions) {
        m_ctx->SetNodePosition(id, pos);
    }
}

void MoveNodesCommand::Undo() {
    if (!m_ctx) return;
    for (const auto& [id, pos] : m_oldPositions) {
        m_ctx->SetNodePosition(id, pos);
    }
}

bool MoveNodesCommand::CanMergeWith(const ICommand* other) const {
    const auto* o = dynamic_cast<const MoveNodesCommand*>(other);
    return o != nullptr && o->m_ctx == m_ctx;
}

void MoveNodesCommand::MergeWith(const ICommand* other) {
    const auto* o = dynamic_cast<const MoveNodesCommand*>(other);
    if (o) {
        m_newPositions = o->m_newPositions;
    }
}

} // namespace nf::ui
