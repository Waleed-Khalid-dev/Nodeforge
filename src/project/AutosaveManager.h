#pragma once

#include "ProjectFile.h"
#include <string>

namespace nf::project {

class AutosaveManager {
public:
    AutosaveManager(float intervalSeconds = 60.0f);
    ~AutosaveManager() = default;

    // Advances the autosave timer. Performs save if dirty and interval elapsed.
    void Update(float deltaTimeSeconds, ProjectData& project);

    // Forces an immediate autosave of the active project
    bool PerformAutosave(ProjectData& project);

    // Gets the destination path for an autosave file
    std::string GetAutosaveFilePath(const std::string& projectFilePath) const;

    // Checks if an autosave file exists and is newer than the saved project
    bool HasNewerAutosave(const std::string& projectFilePath, std::string& outAutosavePath) const;

    // Discards/deletes the autosave file
    void DiscardAutosave(const std::string& projectFilePath);

    void SetInterval(float seconds) { m_intervalSeconds = seconds; }
    float GetInterval() const { return m_intervalSeconds; }

private:
    float m_intervalSeconds = 60.0f;
    float m_elapsedSeconds = 0.0f;
};

} // namespace nf::project
