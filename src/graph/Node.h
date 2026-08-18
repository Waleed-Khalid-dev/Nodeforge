#pragma once

#include "Pin.h"
#include "CookContext.h"
#include "../param/ParameterGroup.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace nf {

class Graph;

using NodeId = uint64_t;

class Node {
public:
    Node(NodeId id, const std::string& name, const std::string& typeName);
    virtual ~Node() = default;

    NodeId GetId() const { return m_id; }
    const std::string& GetName() const { return m_name; }
    void SetName(const std::string& name) { m_name = name; }
    const std::string& GetTypeName() const { return m_typeName; }

    void SetGraph(Graph* graph) { m_graph = graph; }
    Graph* GetGraph() const { return m_graph; }

    // Pin Management
    Pin* AddInputPin(const std::string& name, PinType type, const PinValue& defaultValue = PinValue{});
    Pin* AddOutputPin(const std::string& name, PinType type, const PinValue& defaultValue = PinValue{});

    Pin* GetInputPin(const std::string& name);
    Pin* GetOutputPin(const std::string& name);
    Pin* GetInputPin(size_t index);
    Pin* GetOutputPin(size_t index);

    const std::vector<std::unique_ptr<Pin>>& GetInputPins() const { return m_inputPins; }
    const std::vector<std::unique_ptr<Pin>>& GetOutputPins() const { return m_outputPins; }

    // Parameter Management
    ParameterGroup& GetParams() { return m_params; }
    const ParameterGroup& GetParams() const { return m_params; }

    void SetParam(const std::string& name, const PinValue& value);
    const PinValue& GetParam(const std::string& name) const;
    bool HasParam(const std::string& name) const;

    // Dirty & Cook Pipeline
    bool IsDirty() const { return m_isDirty; }
    uint64_t GetLastCookFrame() const { return m_lastCookFrame; }
    void MarkDirty();
    void ClearDirty() { m_isDirty = false; }

    bool EnsureCooked(const CookContext& context);
    virtual bool Cook(const CookContext& context) = 0;

    // Upstream & Downstream traversal
    std::vector<Node*> GetUpstreamNodes() const;
    const std::vector<Node*>& GetDownstreamNodes() const { return m_downstreamNodes; }
    void AddDownstreamNode(Node* node) { m_downstreamNodes.push_back(node); }
    void RemoveDownstreamNode(Node* node);

protected:
    NodeId m_id;
    std::string m_name;
    std::string m_typeName;
    Graph* m_graph = nullptr;

    std::vector<std::unique_ptr<Pin>> m_inputPins;
    std::vector<std::unique_ptr<Pin>> m_outputPins;
    ParameterGroup m_params;
    std::vector<Node*> m_downstreamNodes;

    bool m_isDirty = true;
    uint64_t m_lastCookFrame = UINT64_MAX;
};

} // namespace nf
