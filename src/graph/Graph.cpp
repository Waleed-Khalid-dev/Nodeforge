#include "Graph.h"
#include "../gpu/Device.h"
#include <spdlog/spdlog.h>
#include <queue>
#include <unordered_set>

namespace nf {

Graph::~Graph() {
    Clear();
}

void Graph::Clear() {
    m_wires.clear();
    m_nodes.clear();
    m_nextNodeId = 1;
    m_nextWireId = 1;
}

Node* Graph::AddNode(std::unique_ptr<Node> node) {
    if (!node) return nullptr;
    node->SetGraph(this);
    Node* ptr = node.get();
    m_nodes[node->GetId()] = std::move(node);
    return ptr;
}

bool Graph::RemoveNode(NodeId id) {
    auto it = m_nodes.find(id);
    if (it == m_nodes.end()) return false;

    Node* node = it->second.get();

    // Disconnect all wires attached to this node's pins
    std::vector<WireId> wiresToRemove;
    for (const auto& [wireId, wire] : m_wires) {
        if (wire->GetFromPin()->GetNode() == node || wire->GetToPin()->GetNode() == node) {
            wiresToRemove.push_back(wireId);
        }
    }
    for (WireId wid : wiresToRemove) {
        Disconnect(wid);
    }

    m_nodes.erase(it);
    return true;
}

Node* Graph::GetNode(NodeId id) const {
    auto it = m_nodes.find(id);
    if (it != m_nodes.end()) return it->second.get();
    return nullptr;
}

Node* Graph::FindNode(const std::string& name) const {
    for (const auto& [id, node] : m_nodes) {
        if (node->GetName() == name) return node.get();
    }
    return nullptr;
}

bool Graph::WouldCreateCycle(const Pin* fromOutPin, const Pin* toInPin) const {
    if (!fromOutPin || !toInPin) return true;
    Node* srcNode = fromOutPin->GetNode();
    Node* dstNode = toInPin->GetNode();

    if (srcNode == dstNode) return true; // Self-loop

    // DFS check: Can we reach srcNode starting from dstNode?
    std::unordered_set<Node*> visited;
    std::queue<Node*> queue;
    queue.push(dstNode);
    visited.insert(dstNode);

    while (!queue.empty()) {
        Node* current = queue.front();
        queue.pop();

        if (current == srcNode) {
            return true; // Found path from dstNode to srcNode -> cycle!
        }

        for (Node* downstream : current->GetDownstreamNodes()) {
            if (visited.find(downstream) == visited.end()) {
                visited.insert(downstream);
                queue.push(downstream);
            }
        }
    }

    return false;
}

Wire* Graph::Connect(Pin* fromOutPin, Pin* toInPin, std::string* outError) {
    if (!fromOutPin || !toInPin) {
        if (outError) *outError = "Null pin pointer provided";
        return nullptr;
    }

    if (!fromOutPin->CanConnectTo(toInPin)) {
        if (outError) *outError = "Invalid pin connection: direction or type mismatch";
        return nullptr;
    }

    if (WouldCreateCycle(fromOutPin, toInPin)) {
        if (outError) *outError = fmt::format("Connection from '{}' to '{}' creates a cycle", 
                                              fromOutPin->GetNode()->GetName(), 
                                              toInPin->GetNode()->GetName());
        spdlog::warn("Cycle rejected: {} -> {}", fromOutPin->GetNode()->GetName(), toInPin->GetNode()->GetName());
        return nullptr;
    }

    // If input pin already has a connection, disconnect it first
    Disconnect(toInPin);

    // Register connection in pins
    fromOutPin->AddConnection(toInPin);
    toInPin->AddConnection(fromOutPin);
    fromOutPin->GetNode()->AddDownstreamNode(toInPin->GetNode());

    WireId wid = m_nextWireId++;
    auto wire = std::make_unique<Wire>(wid, fromOutPin, toInPin);
    Wire* ptr = wire.get();
    m_wires[wid] = std::move(wire);

    // Mark downstream node dirty
    toInPin->GetNode()->MarkDirty();

    return ptr;
}

bool Graph::Disconnect(WireId id) {
    auto it = m_wires.find(id);
    if (it == m_wires.end()) return false;

    Wire* wire = it->second.get();
    Pin* fromPin = wire->GetFromPin();
    Pin* toPin = wire->GetToPin();

    if (fromPin) {
        fromPin->RemoveConnection(toPin);
        if (toPin) {
            fromPin->GetNode()->RemoveDownstreamNode(toPin->GetNode());
        }
    }
    if (toPin) {
        toPin->RemoveConnection(fromPin);
        toPin->GetNode()->MarkDirty();
    }

    m_wires.erase(it);
    return true;
}

bool Graph::Disconnect(Pin* inputPin) {
    if (!inputPin) return false;
    WireId targetWireId = 0;
    for (const auto& [id, wire] : m_wires) {
        if (wire->GetToPin() == inputPin) {
            targetWireId = id;
            break;
        }
    }
    if (targetWireId != 0) {
        return Disconnect(targetWireId);
    }
    return false;
}

std::vector<Node*> Graph::GetTopologicalOrder() const {
    std::vector<Node*> order;
    std::unordered_map<Node*, int> inDegree;

    for (const auto& [id, node] : m_nodes) {
        inDegree[node.get()] = 0;
    }

    for (const auto& [id, node] : m_nodes) {
        for (Node* downstream : node->GetDownstreamNodes()) {
            inDegree[downstream]++;
        }
    }

    std::queue<Node*> zeroInDegreeQueue;
    for (const auto& [node, deg] : inDegree) {
        if (deg == 0) {
            zeroInDegreeQueue.push(node);
        }
    }

    while (!zeroInDegreeQueue.empty()) {
        Node* current = zeroInDegreeQueue.front();
        zeroInDegreeQueue.pop();
        order.push_back(current);

        for (Node* downstream : current->GetDownstreamNodes()) {
            inDegree[downstream]--;
            if (inDegree[downstream] == 0) {
                zeroInDegreeQueue.push(downstream);
            }
        }
    }

    if (order.size() != m_nodes.size()) {
        spdlog::error("Graph contains unresolved cycles during topological sort!");
    }

    return order;
}

bool Graph::CookAll(const CookContext& context) {
    auto topoOrder = GetTopologicalOrder();
    bool allSuccess = true;

    if (context.gpuDevice && context.commandBuffer == VK_NULL_HANDLE) {
        VkCommandBuffer cmd = context.gpuDevice->BeginSingleTimeCommands();
        CookContext gpuCtx = context;
        gpuCtx.commandBuffer = cmd;

        for (Node* node : topoOrder) {
            if (!node->EnsureCooked(gpuCtx)) {
                allSuccess = false;
            }
        }

        context.gpuDevice->EndSingleTimeCommands(cmd);
        return allSuccess;
    }

    for (Node* node : topoOrder) {
        if (!node->EnsureCooked(context)) {
            allSuccess = false;
        }
    }

    return allSuccess;
}

bool Graph::CookNode(Node* node, const CookContext& context) {
    if (!node) return false;
    if (context.gpuDevice && context.commandBuffer == VK_NULL_HANDLE) {
        VkCommandBuffer cmd = context.gpuDevice->BeginSingleTimeCommands();
        CookContext gpuCtx = context;
        gpuCtx.commandBuffer = cmd;

        bool success = node->EnsureCooked(gpuCtx);
        context.gpuDevice->EndSingleTimeCommands(cmd);
        return success;
    }
    return node->EnsureCooked(context);
}

} // namespace nf
