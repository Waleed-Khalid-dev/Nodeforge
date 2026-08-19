#include "AutosaveManager.h"
#include "ProjectSerializer.h"
#include "PathUtils.h"
#include <filesystem>
#include <spdlog/spdlog.h>

namespace nf::project {

AutosaveManager::AutosaveManager(float intervalSeconds)
    : m_intervalSeconds(intervalSeconds), m_elapsedSeconds(0.0f) {
}

void AutosaveManager::Update(float deltaTimeSeconds, ProjectData& project) {
    if (!project.isDirty) {
        m_elapsedSeconds = 0.0f;
        return;
    }

    m_elapsedSeconds += deltaTimeSeconds;
    if (m_elapsedSeconds >= m_intervalSeconds) {
        m_elapsedSeconds = 0.0f;
        PerformAutosave(project);
    }
}

std::string AutosaveManager::GetAutosaveFilePath(const std::string& projectFilePath) const {
    if (!projectFilePath.empty()) {
        return projectFilePath + ".autosave";
    }

#ifdef _WIN32
    const char* appData = std::getenv("APPDATA");
    if (appData) {
        return PathUtils::NormalizePath(std::string(appData) + "/NodeForge/autosave/untitled.nfp.autosave");
    }
#endif
    return "untitled.nfp.autosave";
}

bool AutosaveManager::PerformAutosave(ProjectData& project) {
    std::string autosavePath = GetAutosaveFilePath(project.filePath);
    std::string err;
    if (ProjectSerializer::SaveToFile(autosavePath, project, &err)) {
        spdlog::info("[Autosave] Successfully saved backup to: {}", autosavePath);
        return true;
    } else {
        spdlog::warn("[Autosave] Failed to save backup: {}", err);
        return false;
    }
}

bool AutosaveManager::HasNewerAutosave(const std::string& projectFilePath, std::string& outAutosavePath) const {
    outAutosavePath = GetAutosaveFilePath(projectFilePath);
    if (!std::filesystem::exists(outAutosavePath)) {
        return false;
    }

    if (projectFilePath.empty() || !std::filesystem::exists(projectFilePath)) {
        return true;
    }

    try {
        auto autoTime = std::filesystem::last_write_time(outAutosavePath);
        auto projTime = std::filesystem::last_write_time(projectFilePath);
        return autoTime > projTime;
    } catch (...) {
        return false;
    }
}

void AutosaveManager::DiscardAutosave(const std::string& projectFilePath) {
    std::string path = GetAutosaveFilePath(projectFilePath);
    try {
        if (std::filesystem::exists(path)) {
            std::filesystem::remove(path);
        }
    } catch (...) {
    }
}

} // namespace nf::project
