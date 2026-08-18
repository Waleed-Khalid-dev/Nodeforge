#pragma once

#include "Node.h"
#include "Wire.h"
#include "CookContext.h"
#include <unordered_map>
#include <memory>
#include <vector>
#include <string>

namespace nf {

class Graph {
public:
    Graph() = default;
    ~Graph();

    // Disable copy
    Graph(const Graph&) = delete;
    Graph& operator=(const Graph&) = delete;

    // Node Operations
    template <typename T, typename... Args>
    T* CreateNode(const std::string& name, Args&&... args) {
        NodeId id = m_nextNodeId++;
        auto node = std::make_unique<T>(id, name, std::forward<Args>(args)...);
        node->SetGraph(this);
        T* ptr = node.get();
        m_nodes[id] = std::move(node);
        return ptr;
    }

    NodeId GenerateNodeId() { return m_nextNodeId++; }
    Node* AddNode(std::unique_ptr<Node> node);
    bool RemoveNode(NodeId id);
    Node* GetNode(NodeId id) const;
    Node* FindNode(const std::string& name) const;
    const std::unordered_map<NodeId, std::unique_ptr<Node>>& GetNodes() const { return m_nodes; }

    // Wire Operations
    Wire* Connect(Pin* fromOutPin, Pin* toInPin, std::string* outError = nullptr);
    bool Disconnect(WireId id);
    bool Disconnect(Pin* inputPin);
    const std::unordered_map<WireId, std::unique_ptr<Wire>>& GetWires() const { return m_wires; }

    // Graph Analysis & Evaluation
    bool WouldCreateCycle(const Pin* fromOutPin, const Pin* toInPin) const;
    std::vector<Node*> GetTopologicalOrder() const;
    bool CookAll(const CookContext& context);
    bool CookNode(Node* node, const CookContext& context);

    void Clear();

private:
    NodeId m_nextNodeId = 1;
    WireId m_nextWireId = 1;
    std::unordered_map<NodeId, std::unique_ptr<Node>> m_nodes;
    std::unordered_map<WireId, std::unique_ptr<Wire>> m_wires;
};

} // namespace nf
