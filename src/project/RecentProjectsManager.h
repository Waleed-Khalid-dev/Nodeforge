#pragma once

#include <string>
#include <vector>

namespace nf::project {

class RecentProjectsManager {
public:
    static RecentProjectsManager& Instance();

    void Load();
    void Save();

    void AddRecentProject(const std::string& filePath);
    const std::vector<std::string>& GetRecentProjects() const { return m_recentFiles; }
    void Clear();

private:
    RecentProjectsManager();
    ~RecentProjectsManager() = default;

    std::string GetConfigFilePath() const;

    std::vector<std::string> m_recentFiles;
    const size_t m_maxRecentFiles = 10;
};

} // namespace nf::project
