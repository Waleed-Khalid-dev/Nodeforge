#pragma once

#include "ProjectFile.h"
#include "../graph/NodeRegistry.h"
#include <nlohmann/json.hpp>
#include <string>

namespace nf::project {

class ProjectSerializer {
public:
    // Serializes a ProjectData to a .nfp file on disk
    static bool SaveToFile(const std::string& filePath, const ProjectData& project, std::string* outError = nullptr);

    // Loads a .nfp file from disk into ProjectData
    static bool LoadFromFile(const std::string& filePath, ProjectData& outProject, std::string* outError = nullptr);

    // Converts ProjectData to JSON object
    static nlohmann::json SerializeProject(const ProjectData& project, const std::string& projectDir = "");

    // Parses JSON object into ProjectData
    static bool DeserializeProject(const nlohmann::json& jsonDoc, ProjectData& outProject, const std::string& projectDir = "", std::string* outError = nullptr);

    // Helper to serialize an individual Graph and its node positions
    static nlohmann::json SerializeGraph(const Graph& graph, const std::unordered_map<NodeId, glm::vec2>& nodePositions, const std::string& projectDir);

    // Helper to deserialize an individual Graph and its node positions
    static bool DeserializeGraph(const nlohmann::json& graphJson, Graph& outGraph, std::unordered_map<NodeId, glm::vec2>& outNodePositions, const std::string& projectDir, std::string* outError = nullptr);
};

} // namespace nf::project
