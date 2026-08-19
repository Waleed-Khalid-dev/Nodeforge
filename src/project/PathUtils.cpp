#include "PathUtils.h"
#include <algorithm>
#include <system_error>

namespace nf::project {

std::string PathUtils::NormalizePath(const std::string& path) {
    std::string normalized = path;
    std::replace(normalized.begin(), normalized.end(), '\\', '/');
    while (!normalized.empty() && normalized.back() == '/' && normalized.size() > 1) {
        normalized.pop_back();
    }
    return normalized;
}

std::string PathUtils::ToProjectRelative(const std::string& targetPath, const std::string& projectDir) {
    if (targetPath.empty() || projectDir.empty()) {
        return NormalizePath(targetPath);
    }

    try {
        std::filesystem::path target(targetPath);
        std::filesystem::path base(projectDir);

        if (!target.is_absolute()) {
            return NormalizePath(targetPath);
        }

        std::error_code ec;
        auto rel = std::filesystem::relative(target, base, ec);
        if (!ec) {
            return NormalizePath(rel.string());
        }
    } catch (...) {
    }

    return NormalizePath(targetPath);
}

std::string PathUtils::ToAbsolute(const std::string& relativePath, const std::string& projectDir) {
    if (relativePath.empty()) return "";
    
    try {
        std::filesystem::path rel(relativePath);
        if (rel.is_absolute()) {
            return NormalizePath(relativePath);
        }

        if (projectDir.empty()) {
            return NormalizePath(std::filesystem::absolute(rel).string());
        }

        std::filesystem::path base(projectDir);
        auto abs = std::filesystem::weakly_canonical(base / rel);
        return NormalizePath(abs.string());
    } catch (...) {
        return NormalizePath(relativePath);
    }
}

std::string PathUtils::GetDirectory(const std::string& filePath) {
    if (filePath.empty()) return "";
    try {
        std::filesystem::path p(filePath);
        return NormalizePath(p.parent_path().string());
    } catch (...) {
        return "";
    }
}

bool PathUtils::EnsureDirectoryExists(const std::string& dirPath) {
    if (dirPath.empty()) return true;
    try {
        std::error_code ec;
        return std::filesystem::create_directories(dirPath, ec) || std::filesystem::exists(dirPath);
    } catch (...) {
        return false;
    }
}

} // namespace nf::project
