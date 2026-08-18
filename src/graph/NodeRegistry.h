#pragma once

#include "NodeTypeInfo.h"
#include <unordered_map>
#include <vector>
#include <memory>

namespace nf {

class NodeRegistry {
public:
    static NodeRegistry& Instance();

    void Register(const NodeTypeInfo& info);

    template <typename T>
    void RegisterNodeType(const std::string& typeName, NodeFamily family, const std::string& category, const std::string& description) {
        NodeTypeInfo info;
        info.typeName = typeName;
        info.family = family;
        info.category = category;
        info.description = description;
        info.factory = [](NodeId id, const std::string& name) -> std::unique_ptr<Node> {
            return std::make_unique<T>(id, name);
        };
        Register(info);
    }

    std::unique_ptr<Node> CreateNode(const std::string& typeName, NodeId id, const std::string& name) const;
    const NodeTypeInfo* GetTypeInfo(const std::string& typeName) const;
    std::vector<const NodeTypeInfo*> GetTypesByFamily(NodeFamily family) const;
    const std::unordered_map<std::string, NodeTypeInfo>& GetAllTypes() const { return m_registry; }

    void Clear();

private:
    std::unordered_map<std::string, NodeTypeInfo> m_registry;
};

} // namespace nf
