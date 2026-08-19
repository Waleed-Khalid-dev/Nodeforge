#include "ComponentSerializer.h"
#include "ProjectSerializer.h"
#include "PathUtils.h"
#include <fstream>
#include <spdlog/spdlog.h>

namespace nf::project {

bool ComponentSerializer::ExportComponent(const std::string& filePath, ContainerComp* comp, std::string* outError) {
    if (!comp || !comp->GetInnerGraph()) {
        if (outError) *outError = "Cannot export null container component";
        return false;
    }

    std::string projectDir = PathUtils::GetDirectory(filePath);
    PathUtils::EnsureDirectoryExists(projectDir);

    try {
        nlohmann::json root;
        root["format"] = "nodeforge_component_v1";
        root["schema_version"] = 1;
        root["name"] = comp->GetName();

        // Serialize inner graph
        std::unordered_map<NodeId, glm::vec2> innerPos;
        root["inner_graph"] = ProjectSerializer::SerializeGraph(*comp->GetInnerGraph(), innerPos, projectDir);

        std::ofstream file(filePath);
        if (!file.is_open()) {
            if (outError) *outError = "Failed to open component file for writing: " + filePath;
            return false;
        }

        file << root.dump(2);
        file.close();
        return true;
    } catch (const std::exception& e) {
        if (outError) *outError = e.what();
        return false;
    }
}

ContainerComp* ComponentSerializer::ImportComponent(const std::string& filePath, Graph* targetGraph, const glm::vec2& /*spawnPos*/, std::string* outError) {
    if (!targetGraph) {
        if (outError) *outError = "Target graph cannot be null";
        return nullptr;
    }

    try {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            if (outError) *outError = "Failed to open component file for reading: " + filePath;
            return nullptr;
        }

        nlohmann::json root;
        file >> root;
        file.close();

        if (!root.contains("format") || root["format"] != "nodeforge_component_v1") {
            if (outError) *outError = "Invalid component format: missing nodeforge_component_v1 header";
            return nullptr;
        }

        std::string compName = root.value("name", "Comp");
        NodeId newId = targetGraph->GenerateNodeId();
        auto compNode = std::make_unique<ContainerComp>(newId, compName);

        if (root.contains("inner_graph") && root["inner_graph"].is_object()) {
            auto innerGraph = std::make_unique<Graph>();
            std::unordered_map<NodeId, glm::vec2> innerPos;
            std::string projectDir = PathUtils::GetDirectory(filePath);
            if (ProjectSerializer::DeserializeGraph(root["inner_graph"], *innerGraph, innerPos, projectDir, outError)) {
                compNode->SetInnerGraph(std::move(innerGraph));
            }
        }

        ContainerComp* rawPtr = compNode.get();
        targetGraph->AddNode(std::move(compNode));
        return rawPtr;
    } catch (const std::exception& e) {
        if (outError) *outError = e.what();
        return nullptr;
    }
}

} // namespace nf::project
