#pragma once

#include "ICommand.h"
#include "../../graph/Graph.h"
#include "../../graph/NodeRegistry.h"
#include <glm/vec2.hpp>
#include <unordered_map>

namespace nf::ui {

class EditorContext;

class CreateNodeCommand : public ICommand {
public:
    CreateNodeCommand(EditorContext* ctx, const std::string& typeName, const std::string& name, const glm::vec2& pos);
    virtual void Execute() override;
    virtual void Undo() override;
    virtual std::string GetName() const override { return "Create Node: " + m_typeName; }
    NodeId GetCreatedNodeId() const { return m_createdNodeId; }

private:
    EditorContext* m_ctx;
    std::string m_typeName;
    std::string m_name;
    glm::vec2 m_position;
    NodeId m_createdNodeId = 0;
};

class DeleteNodeCommand : public ICommand {
public:
    DeleteNodeCommand(EditorContext* ctx, NodeId nodeId);
    virtual void Execute() override;
    virtual void Undo() override;
    virtual std::string GetName() const override { return "Delete Node"; }

private:
    EditorContext* m_ctx;
    NodeId m_nodeId;
    std::string m_typeName;
    std::string m_nodeName;
    glm::vec2 m_position;
};

class MoveNodesCommand : public ICommand {
public:
    MoveNodesCommand(EditorContext* ctx, const std::unordered_map<NodeId, glm::vec2>& oldPositions, const std::unordered_map<NodeId, glm::vec2>& newPositions);
    virtual void Execute() override;
    virtual void Undo() override;
    virtual std::string GetName() const override { return "Move Nodes"; }
    virtual bool CanMergeWith(const ICommand* other) const override;
    virtual void MergeWith(const ICommand* other) override;

private:
    EditorContext* m_ctx;
    std::unordered_map<NodeId, glm::vec2> m_oldPositions;
    std::unordered_map<NodeId, glm::vec2> m_newPositions;
};

} // namespace nf::ui
