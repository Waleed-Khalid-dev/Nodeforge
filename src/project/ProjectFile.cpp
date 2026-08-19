#include "ProjectFile.h"
#include <chrono>
#include <iomanip>
#include <sstream>

namespace nf::project {

static std::string GetCurrentISOTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::gmtime(&in_time_t), "%Y-%m-%dT%H:%M:%SZ");
    return ss.str();
}

ProjectData::ProjectData() {
    Reset();
}

void ProjectData::Reset() {
    metadata.name = "Untitled";
    metadata.author = "User";
    metadata.description = "";
    metadata.schemaVersion = 1;
    metadata.engineVersion = "0.1.0";
    metadata.createdTimestamp = GetCurrentISOTimestamp();
    metadata.modifiedTimestamp = metadata.createdTimestamp;

    timeline = TimelineState{};
    viewport = CanvasViewState{};
    rootGraph = std::make_unique<Graph>();
    nodePositions.clear();
    filePath = "";
    isDirty = false;
}

} // namespace nf::project
