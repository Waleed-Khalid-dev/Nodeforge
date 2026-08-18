#pragma once

#include "Graph.h"
#include "NodeRegistry.h"
#include <nlohmann/json.hpp>

namespace nf {

class GraphSerializer {
public:
    static nlohmann::json Serialize(const Graph& graph);
    static bool Deserialize(Graph& graph, const nlohmann::json& json, const NodeRegistry& registry, std::string* outError = nullptr);
};

} // namespace nf
