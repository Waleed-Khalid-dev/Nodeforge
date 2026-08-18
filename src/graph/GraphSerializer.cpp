#include "GraphSerializer.h"
#include <spdlog/spdlog.h>

namespace nf {

static nlohmann::json SerializePinValue(const PinValue& val) {
    nlohmann::json j;
    if (val.Is<float>()) {
        j["type"] = "float";
        j["val"] = val.Get<float>();
    } else if (val.Is<int32_t>()) {
        j["type"] = "int";
        j["val"] = val.Get<int32_t>();
    } else if (val.Is<bool>()) {
        j["type"] = "bool";
        j["val"] = val.Get<bool>();
    } else if (val.Is<std::string>()) {
        j["type"] = "string";
        j["val"] = val.Get<std::string>();
    } else if (val.Is<glm::vec2>()) {
        auto v = val.Get<glm::vec2>();
        j["type"] = "vec2";
        j["val"] = { v.x, v.y };
    } else if (val.Is<glm::vec3>()) {
        auto v = val.Get<glm::vec3>();
        j["type"] = "vec3";
        j["val"] = { v.x, v.y, v.z };
    } else if (val.Is<glm::vec4>()) {
        auto v = val.Get<glm::vec4>();
        j["type"] = "vec4";
        j["val"] = { v.x, v.y, v.z, v.w };
    } else {
        j["type"] = "empty";
    }
    return j;
}

static PinValue DeserializePinValue(const nlohmann::json& j) {
    if (!j.contains("type") || !j.contains("val")) return PinValue{};
    std::string type = j["type"].get<std::string>();
    if (type == "float") return PinValue(j["val"].get<float>());
    if (type == "int") return PinValue(j["val"].get<int32_t>());
    if (type == "bool") return PinValue(j["val"].get<bool>());
    if (type == "string") return PinValue(j["val"].get<std::string>());
    if (type == "vec2") return PinValue(glm::vec2(j["val"][0].get<float>(), j["val"][1].get<float>()));
    if (type == "vec3") return PinValue(glm::vec3(j["val"][0].get<float>(), j["val"][1].get<float>(), j["val"][2].get<float>()));
    if (type == "vec4") return PinValue(glm::vec4(j["val"][0].get<float>(), j["val"][1].get<float>(), j["val"][2].get<float>(), j["val"][3].get<float>()));
    return PinValue{};
}

nlohmann::json GraphSerializer::Serialize(const Graph& graph) {
    nlohmann::json root;
    root["format"] = "nodeforge_graph_v1";

    nlohmann::json nodesJson = nlohmann::json::array();
    for (const auto& [id, node] : graph.GetNodes()) {
        nlohmann::json nodeObj;
        nodeObj["id"] = id;
        nodeObj["name"] = node->GetName();
        nodeObj["type"] = node->GetTypeName();

        nlohmann::json paramsObj = nlohmann::json::object();
        for (const auto& [paramName, paramVal] : node->GetAllParams()) {
            paramsObj[paramName] = SerializePinValue(paramVal);
        }
        nodeObj["params"] = paramsObj;
        nodesJson.push_back(nodeObj);
    }
    root["nodes"] = nodesJson;

    nlohmann::json wiresJson = nlohmann::json::array();
    for (const auto& [id, wire] : graph.GetWires()) {
        nlohmann::json wireObj;
        wireObj["id"] = id;
        wireObj["from_node"] = wire->GetFromPin()->GetNode()->GetId();
        wireObj["from_pin"] = wire->GetFromPin()->GetName();
        wireObj["to_node"] = wire->GetToPin()->GetNode()->GetId();
        wireObj["to_pin"] = wire->GetToPin()->GetName();
        wiresJson.push_back(wireObj);
    }
    root["wires"] = wiresJson;

    return root;
}

bool GraphSerializer::Deserialize(Graph& graph, const nlohmann::json& json, const NodeRegistry& registry, std::string* outError) {
    if (!json.contains("format") || json["format"] != "nodeforge_graph_v1") {
        if (outError) *outError = "Unsupported graph serialization format";
        return false;
    }

    graph.Clear();

    // Map serialized ID to created Node*
    std::unordered_map<NodeId, Node*> nodeMap;

    if (json.contains("nodes") && json["nodes"].is_array()) {
        for (const auto& nodeObj : json["nodes"]) {
            NodeId id = nodeObj["id"].get<NodeId>();
            std::string name = nodeObj["name"].get<std::string>();
            std::string type = nodeObj["type"].get<std::string>();

            auto node = registry.CreateNode(type, id, name);
            if (!node) {
                if (outError) *outError = fmt::format("Unknown node type '{}'", type);
                return false;
            }

            if (nodeObj.contains("params") && nodeObj["params"].is_object()) {
                for (auto& [paramName, paramJson] : nodeObj["params"].items()) {
                    node->SetParam(paramName, DeserializePinValue(paramJson));
                }
            }

            Node* ptr = graph.AddNode(std::move(node));
            nodeMap[id] = ptr;
        }
    }

    if (json.contains("wires") && json["wires"].is_array()) {
        for (const auto& wireObj : json["wires"]) {
            NodeId fromNodeId = wireObj["from_node"].get<NodeId>();
            std::string fromPinName = wireObj["from_pin"].get<std::string>();
            NodeId toNodeId = wireObj["to_node"].get<NodeId>();
            std::string toPinName = wireObj["to_pin"].get<std::string>();

            Node* fromNode = nodeMap[fromNodeId];
            Node* toNode = nodeMap[toNodeId];

            if (!fromNode || !toNode) {
                if (outError) *outError = "Wire references invalid node ID";
                return false;
            }

            Pin* fromPin = fromNode->GetOutputPin(fromPinName);
            Pin* toPin = toNode->GetInputPin(toPinName);

            if (!fromPin || !toPin) {
                if (outError) *outError = "Wire references non-existent pin";
                return false;
            }

            std::string connectErr;
            if (!graph.Connect(fromPin, toPin, &connectErr)) {
                if (outError) *outError = connectErr;
                return false;
            }
        }
    }

    return true;
}

} // namespace nf
