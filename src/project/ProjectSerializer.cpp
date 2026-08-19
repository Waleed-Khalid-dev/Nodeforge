#include "ProjectSerializer.h"
#include "PathUtils.h"
#include "../operators/comp/ContainerComp.h"
#include "../operators/comp/InOp.h"
#include "../operators/comp/OutOp.h"
#include "../param/Parameter.h"
#include <fstream>
#include <spdlog/spdlog.h>

namespace nf::project {

static nlohmann::json SerializePinValue(const PinValue& val, bool isPath, const std::string& projectDir) {
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
        std::string str = val.Get<std::string>();
        if (isPath && !projectDir.empty()) {
            str = PathUtils::ToProjectRelative(str, projectDir);
        }
        j["val"] = str;
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

static PinValue DeserializePinValue(const nlohmann::json& j, bool isPath, const std::string& projectDir) {
    if (!j.contains("type") || !j.contains("val")) return PinValue{};
    std::string type = j["type"].get<std::string>();
    if (type == "float") return PinValue(j["val"].get<float>());
    if (type == "int") return PinValue(j["val"].get<int32_t>());
    if (type == "bool") return PinValue(j["val"].get<bool>());
    if (type == "string") {
        std::string str = j["val"].get<std::string>();
        if (isPath && !projectDir.empty()) {
            str = PathUtils::ToAbsolute(str, projectDir);
        }
        return PinValue(str);
    }
    if (type == "vec2") return PinValue(glm::vec2(j["val"][0].get<float>(), j["val"][1].get<float>()));
    if (type == "vec3") return PinValue(glm::vec3(j["val"][0].get<float>(), j["val"][1].get<float>(), j["val"][2].get<float>()));
    if (type == "vec4") return PinValue(glm::vec4(j["val"][0].get<float>(), j["val"][1].get<float>(), j["val"][2].get<float>(), j["val"][3].get<float>()));
    return PinValue{};
}

nlohmann::json ProjectSerializer::SerializeGraph(const Graph& graph, const std::unordered_map<NodeId, glm::vec2>& nodePositions, const std::string& projectDir) {
    nlohmann::json graphObj;

    nlohmann::json nodesArray = nlohmann::json::array();
    for (const auto& [id, node] : graph.GetNodes()) {
        nlohmann::json nodeObj;
        nodeObj["id"] = id;
        nodeObj["name"] = node->GetName();
        nodeObj["type"] = node->GetTypeName();

        // Node Canvas position
        auto itPos = nodePositions.find(id);
        if (itPos != nodePositions.end()) {
            nodeObj["pos"] = { itPos->second.x, itPos->second.y };
        } else {
            nodeObj["pos"] = { 0.0f, 0.0f };
        }

        // Parameters
        nlohmann::json paramsObj = nlohmann::json::object();
        for (const auto& param : node->GetParams().GetAll()) {
            nlohmann::json paramObj;
            paramObj["mode"] = (param->GetMode() == ParamMode::Expression) ? "expression" : "constant";
            paramObj["expr"] = param->GetExpression();
            bool isPath = (param->GetType() == ParamType::Filepath || param->GetName().find("path") != std::string::npos || param->GetName().find("file") != std::string::npos);
            paramObj["val"] = SerializePinValue(param->GetConstantValue(), isPath, projectDir);
            paramsObj[param->GetName()] = paramObj;
        }
        nodeObj["params"] = paramsObj;

        // ContainerComp nested subnetwork
        if (auto* comp = dynamic_cast<const ContainerComp*>(node.get())) {
            if (comp->GetInnerGraph()) {
                std::unordered_map<NodeId, glm::vec2> innerPositions;
                nodeObj["inner_graph"] = SerializeGraph(*comp->GetInnerGraph(), innerPositions, projectDir);
            }
        }

        nodesArray.push_back(nodeObj);
    }
    graphObj["nodes"] = nodesArray;

    // Wires
    nlohmann::json wiresArray = nlohmann::json::array();
    for (const auto& [id, wire] : graph.GetWires()) {
        nlohmann::json wireObj;
        wireObj["id"] = id;
        wireObj["from_node"] = wire->GetFromPin()->GetNode()->GetId();
        wireObj["from_pin"] = wire->GetFromPin()->GetName();
        wireObj["to_node"] = wire->GetToPin()->GetNode()->GetId();
        wireObj["to_pin"] = wire->GetToPin()->GetName();
        wiresArray.push_back(wireObj);
    }
    graphObj["wires"] = wiresArray;

    return graphObj;
}

bool ProjectSerializer::DeserializeGraph(const nlohmann::json& graphJson, Graph& outGraph, std::unordered_map<NodeId, glm::vec2>& outNodePositions, const std::string& projectDir, std::string* outError) {
    outGraph.Clear();

    const auto& registry = NodeRegistry::Instance();
    std::unordered_map<NodeId, Node*> nodeMap;

    if (graphJson.contains("nodes") && graphJson["nodes"].is_array()) {
        for (const auto& nodeObj : graphJson["nodes"]) {
            NodeId id = nodeObj.value("id", outGraph.GenerateNodeId());
            std::string name = nodeObj.value("name", "Node");
            std::string type = nodeObj.value("type", "NullTexOp");

            auto node = registry.CreateNode(type, id, name);
            if (!node) {
                if (outError) *outError = "Unknown node type during deserialization: " + type;
                return false;
            }

            // Position
            if (nodeObj.contains("pos") && nodeObj["pos"].is_array() && nodeObj["pos"].size() >= 2) {
                outNodePositions[id] = glm::vec2(nodeObj["pos"][0].get<float>(), nodeObj["pos"][1].get<float>());
            }

            // Parameters
            if (nodeObj.contains("params") && nodeObj["params"].is_object()) {
                for (auto& [paramName, paramJson] : nodeObj["params"].items()) {
                    bool isPath = (paramName.find("path") != std::string::npos || paramName.find("file") != std::string::npos);
                    if (paramJson.is_object() && paramJson.contains("val")) {
                        PinValue val = DeserializePinValue(paramJson["val"], isPath, projectDir);
                        Parameter* p = node->GetParams().Get(paramName);
                        if (p) {
                            p->SetValue(val);
                            if (paramJson.contains("mode") && paramJson["mode"] == "expression") {
                                p->SetExpression(paramJson.value("expr", ""));
                            }
                        } else {
                            node->SetParam(paramName, val);
                            if (paramJson.contains("mode") && paramJson["mode"] == "expression") {
                                Parameter* added = node->GetParams().Get(paramName);
                                if (added) added->SetExpression(paramJson.value("expr", ""));
                            }
                        }
                    }
                }
            }

            // ContainerComp subnetwork recursion
            if (auto* comp = dynamic_cast<ContainerComp*>(node.get())) {
                if (nodeObj.contains("inner_graph") && nodeObj["inner_graph"].is_object()) {
                    auto innerGraph = std::make_unique<Graph>();
                    std::unordered_map<NodeId, glm::vec2> innerPos;
                    if (DeserializeGraph(nodeObj["inner_graph"], *innerGraph, innerPos, projectDir, outError)) {
                        comp->SetInnerGraph(std::move(innerGraph));
                    }
                }
            }

            Node* ptr = outGraph.AddNode(std::move(node));
            nodeMap[id] = ptr;
        }
    }

    // Connect wires
    if (graphJson.contains("wires") && graphJson["wires"].is_array()) {
        for (const auto& wireObj : graphJson["wires"]) {
            NodeId fromNodeId = wireObj.value("from_node", static_cast<NodeId>(0));
            std::string fromPinName = wireObj.value("from_pin", "");
            NodeId toNodeId = wireObj.value("to_node", static_cast<NodeId>(0));
            std::string toPinName = wireObj.value("to_pin", "");

            Node* fromNode = nodeMap[fromNodeId];
            Node* toNode = nodeMap[toNodeId];

            if (fromNode && toNode) {
                Pin* fromPin = fromNode->GetOutputPin(fromPinName);
                Pin* toPin = toNode->GetInputPin(toPinName);
                if (fromPin && toPin) {
                    outGraph.Connect(fromPin, toPin);
                }
            }
        }
    }

    return true;
}

nlohmann::json ProjectSerializer::SerializeProject(const ProjectData& project, const std::string& projectDir) {
    nlohmann::json root;
    root["format"] = "nodeforge_project_v1";
    root["schema_version"] = project.metadata.schemaVersion;

    // Metadata
    nlohmann::json metaObj;
    metaObj["name"] = project.metadata.name;
    metaObj["author"] = project.metadata.author;
    metaObj["description"] = project.metadata.description;
    metaObj["engine_version"] = project.metadata.engineVersion;
    metaObj["created"] = project.metadata.createdTimestamp;
    metaObj["modified"] = project.metadata.modifiedTimestamp;
    root["metadata"] = metaObj;

    // Timeline
    nlohmann::json timelineObj;
    timelineObj["fps"] = project.timeline.fps;
    timelineObj["start_frame"] = project.timeline.startFrame;
    timelineObj["end_frame"] = project.timeline.endFrame;
    timelineObj["current_frame"] = project.timeline.currentFrame;
    timelineObj["bpm"] = project.timeline.bpm;
    timelineObj["is_playing"] = project.timeline.isPlaying;
    root["timeline"] = timelineObj;

    // Viewport
    nlohmann::json viewObj;
    viewObj["pan"] = { project.viewport.pan.x, project.viewport.pan.y };
    viewObj["zoom"] = project.viewport.zoom;
    root["viewport"] = viewObj;

    // Root Graph
    if (project.rootGraph) {
        root["root_graph"] = SerializeGraph(*project.rootGraph, project.nodePositions, projectDir);
    }

    return root;
}

bool ProjectSerializer::DeserializeProject(const nlohmann::json& jsonDoc, ProjectData& outProject, const std::string& projectDir, std::string* outError) {
    if (!jsonDoc.contains("format") || jsonDoc["format"] != "nodeforge_project_v1") {
        if (outError) *outError = "Invalid file format: missing or incompatible nodeforge_project_v1 header";
        return false;
    }

    outProject.Reset();

    // Metadata
    if (jsonDoc.contains("metadata") && jsonDoc["metadata"].is_object()) {
        const auto& m = jsonDoc["metadata"];
        outProject.metadata.name = m.value("name", "Untitled");
        outProject.metadata.author = m.value("author", "User");
        outProject.metadata.description = m.value("description", "");
        outProject.metadata.engineVersion = m.value("engine_version", "0.1.0");
        outProject.metadata.createdTimestamp = m.value("created", "");
        outProject.metadata.modifiedTimestamp = m.value("modified", "");
    }
    outProject.metadata.schemaVersion = jsonDoc.value("schema_version", 1);

    // Timeline
    if (jsonDoc.contains("timeline") && jsonDoc["timeline"].is_object()) {
        const auto& t = jsonDoc["timeline"];
        outProject.timeline.fps = t.value("fps", 60.0f);
        outProject.timeline.startFrame = t.value("start_frame", 1);
        outProject.timeline.endFrame = t.value("end_frame", 600);
        outProject.timeline.currentFrame = t.value("current_frame", 1);
        outProject.timeline.bpm = t.value("bpm", 120.0f);
        outProject.timeline.isPlaying = t.value("is_playing", true);
    }

    // Viewport
    if (jsonDoc.contains("viewport") && jsonDoc["viewport"].is_object()) {
        const auto& v = jsonDoc["viewport"];
        if (v.contains("pan") && v["pan"].is_array() && v["pan"].size() >= 2) {
            outProject.viewport.pan = glm::vec2(v["pan"][0].get<float>(), v["pan"][1].get<float>());
        }
        outProject.viewport.zoom = v.value("zoom", 1.0f);
    }

    // Root Graph
    if (jsonDoc.contains("root_graph") && jsonDoc["root_graph"].is_object()) {
        if (!outProject.rootGraph) {
            outProject.rootGraph = std::make_unique<Graph>();
        }
        if (!DeserializeGraph(jsonDoc["root_graph"], *outProject.rootGraph, outProject.nodePositions, projectDir, outError)) {
            return false;
        }
    }

    return true;
}

bool ProjectSerializer::SaveToFile(const std::string& filePath, const ProjectData& project, std::string* outError) {
    if (filePath.empty()) {
        if (outError) *outError = "File path cannot be empty";
        return false;
    }

    std::string projectDir = PathUtils::GetDirectory(filePath);
    PathUtils::EnsureDirectoryExists(projectDir);

    try {
        nlohmann::json doc = SerializeProject(project, projectDir);
        std::ofstream file(filePath);
        if (!file.is_open()) {
            if (outError) *outError = "Failed to open file for writing: " + filePath;
            return false;
        }
        file << doc.dump(2);
        file.close();
        return true;
    } catch (const std::exception& e) {
        if (outError) *outError = e.what();
        return false;
    }
}

bool ProjectSerializer::LoadFromFile(const std::string& filePath, ProjectData& outProject, std::string* outError) {
    if (filePath.empty()) {
        if (outError) *outError = "File path cannot be empty";
        return false;
    }

    try {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            if (outError) *outError = "Failed to open file for reading: " + filePath;
            return false;
        }

        nlohmann::json doc;
        file >> doc;
        file.close();

        std::string projectDir = PathUtils::GetDirectory(filePath);
        if (!DeserializeProject(doc, outProject, projectDir, outError)) {
            return false;
        }

        outProject.filePath = PathUtils::NormalizePath(filePath);
        outProject.isDirty = false;
        return true;
    } catch (const std::exception& e) {
        if (outError) *outError = e.what();
        return false;
    }
}

} // namespace nf::project
