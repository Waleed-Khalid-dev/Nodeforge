#include "RecentProjectsManager.h"
#include "PathUtils.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <algorithm>
#include <cstdlib>

namespace nf::project {

RecentProjectsManager& RecentProjectsManager::Instance() {
    static RecentProjectsManager s_instance;
    return s_instance;
}

RecentProjectsManager::RecentProjectsManager() {
    Load();
}

std::string RecentProjectsManager::GetConfigFilePath() const {
#ifdef _WIN32
    const char* appData = std::getenv("APPDATA");
    if (appData) {
        return PathUtils::NormalizePath(std::string(appData) + "/NodeForge/recent_projects.json");
    }
#endif
    return "recent_projects.json";
}

void RecentProjectsManager::Load() {
    m_recentFiles.clear();
    std::string configPath = GetConfigFilePath();

    try {
        std::ifstream file(configPath);
        if (file.is_open()) {
            nlohmann::json root;
            file >> root;
            if (root.contains("recent") && root["recent"].is_array()) {
                for (const auto& item : root["recent"]) {
                    if (item.is_string()) {
                        m_recentFiles.push_back(item.get<std::string>());
                    }
                }
            }
        }
    } catch (...) {
    }
}

void RecentProjectsManager::Save() {
    std::string configPath = GetConfigFilePath();
    std::string dir = PathUtils::GetDirectory(configPath);
    PathUtils::EnsureDirectoryExists(dir);

    try {
        nlohmann::json root;
        root["recent"] = m_recentFiles;
        std::ofstream file(configPath);
        if (file.is_open()) {
            file << root.dump(2);
        }
    } catch (...) {
    }
}

void RecentProjectsManager::AddRecentProject(const std::string& filePath) {
    if (filePath.empty()) return;
    std::string norm = PathUtils::NormalizePath(filePath);

    auto it = std::find(m_recentFiles.begin(), m_recentFiles.end(), norm);
    if (it != m_recentFiles.end()) {
        m_recentFiles.erase(it);
    }

    m_recentFiles.insert(m_recentFiles.begin(), norm);
    if (m_recentFiles.size() > m_maxRecentFiles) {
        m_recentFiles.resize(m_maxRecentFiles);
    }
    Save();
}

void RecentProjectsManager::Clear() {
    m_recentFiles.clear();
    Save();
}

} // namespace nf::project
