#pragma once

#include <string>
#include <filesystem>

namespace nf::project {

class PathUtils {
public:
    // Converts an absolute or arbitrary path to a normalized relative path with respect to the project directory
    static std::string ToProjectRelative(const std::string& targetPath, const std::string& projectDir);

    // Converts a project-relative path to an absolute normalized filesystem path
    static std::string ToAbsolute(const std::string& relativePath, const std::string& projectDir);

    // Normalizes directory separators to forward slashes '/' for cross-platform portability
    static std::string NormalizePath(const std::string& path);

    // Gets the directory containing the given file
    static std::string GetDirectory(const std::string& filePath);

    // Ensures parent directories exist
    static bool EnsureDirectoryExists(const std::string& dirPath);
};

} // namespace nf::project
