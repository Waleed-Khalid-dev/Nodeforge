#include "NodeRegistry.h"
#include "Node.h"
#include <spdlog/spdlog.h>

namespace nf {

NodeRegistry& NodeRegistry::Instance() {
    static NodeRegistry instance;
    return instance;
}

void NodeRegistry::Register(const NodeTypeInfo& info) {
    m_registry[info.typeName] = info;
    spdlog::debug("Registered node type: '{}' [{}]", info.typeName, info.category);
}

std::unique_ptr<Node> NodeRegistry::CreateNode(const std::string& typeName, NodeId id, const std::string& name) const {
    auto it = m_registry.find(typeName);
    if (it != m_registry.end() && it->second.factory) {
        return it->second.factory(id, name);
    }
    spdlog::error("Node type '{}' not found in registry", typeName);
    return nullptr;
}

const NodeTypeInfo* NodeRegistry::GetTypeInfo(const std::string& typeName) const {
    auto it = m_registry.find(typeName);
    if (it != m_registry.end()) {
        return &it->second;
    }
    return nullptr;
}

std::vector<const NodeTypeInfo*> NodeRegistry::GetTypesByFamily(NodeFamily family) const {
    std::vector<const NodeTypeInfo*> result;
    for (const auto& [name, info] : m_registry) {
        if (info.family == family) {
            result.push_back(&info);
        }
    }
    return result;
}

void NodeRegistry::Clear() {
    m_registry.clear();
}

} // namespace nf
