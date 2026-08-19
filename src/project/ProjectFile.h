#pragma once

#include "../graph/Graph.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <glm/glm.hpp>

namespace nf::project {

struct ProjectMetadata {
    std::string name = "Untitled";
    std::string author = "User";
    std::string description = "";
    int32_t schemaVersion = 1;
    std::string engineVersion = "0.1.0";
    std::string createdTimestamp = "";
    std::string modifiedTimestamp = "";
};

struct TimelineState {
    float fps = 60.0f;
    int32_t startFrame = 1;
    int32_t endFrame = 600;
    int32_t currentFrame = 1;
    float bpm = 120.0f;
    bool isPlaying = true;
};

struct CanvasViewState {
    glm::vec2 pan = glm::vec2(0.0f, 0.0f);
    float zoom = 1.0f;
};

struct ProjectData {
    ProjectMetadata metadata;
    TimelineState timeline;
    CanvasViewState viewport;
    std::unique_ptr<Graph> rootGraph;
    std::unordered_map<NodeId, glm::vec2> nodePositions;
    std::string filePath = "";
    bool isDirty = false;

    ProjectData();
    ~ProjectData() = default;

    // Reset project to a fresh empty state
    void Reset();
};

} // namespace nf::project
