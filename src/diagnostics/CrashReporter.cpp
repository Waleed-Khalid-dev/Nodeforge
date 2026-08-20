#include "CrashReporter.h"
#include "graph/Graph.h"
#include "graph/GraphSerializer.h"
#include <spdlog/spdlog.h>
#include <fstream>
#include <chrono>
#include <filesystem>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

static LONG WINAPI NodeForgeUnhandledExceptionFilter(EXCEPTION_POINTERS* /*ExceptionInfo*/) {
    nf::CrashReporter::Instance().WriteEmergencyCrashSnapshot("Fatal Unhandled Win32 Exception");
    return EXCEPTION_CONTINUE_SEARCH;
}
#endif

namespace nf {

CrashReporter& CrashReporter::Instance() {
    static CrashReporter s_instance;
    return s_instance;
}

CrashReporter::CrashReporter() = default;

CrashReporter::~CrashReporter() {
    Uninstall();
}

void CrashReporter::Install() {
#ifdef _WIN32
    if (!m_installed) {
        SetUnhandledExceptionFilter(NodeForgeUnhandledExceptionFilter);
        m_installed = true;
        spdlog::info("CrashReporter: Win32 unhandled exception filter installed");
    }
#endif
}

void CrashReporter::Uninstall() {
#ifdef _WIN32
    if (m_installed) {
        SetUnhandledExceptionFilter(nullptr);
        m_installed = false;
    }
#endif
}

void CrashReporter::SetActiveGraph(Graph* graph) {
    m_activeGraph = graph;
}

void CrashReporter::SetCrashDumpPath(const std::string& path) {
    m_dumpPath = path;
}

bool CrashReporter::WriteEmergencyCrashSnapshot(const std::string& reason) {
    if (m_customHook) {
        m_customHook(reason);
    }

    try {
        std::filesystem::path logP(m_logPath);
        if (logP.has_parent_path() && !std::filesystem::exists(logP.parent_path())) {
            std::filesystem::create_directories(logP.parent_path());
        }

        // Write diagnostic crash log
        std::ofstream logFile(m_logPath, std::ios::app);
        if (logFile.is_open()) {
            auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            logFile << "====================================================\n";
            logFile << "NodeForge Emergency Crash Diagnostic Log\n";
            logFile << "Timestamp: " << std::ctime(&now);
            logFile << "Reason: " << reason << "\n";
            if (m_activeGraph) {
                logFile << "Active Nodes in Graph: " << m_activeGraph->GetNodes().size() << "\n";
            }
            logFile << "Emergency Snapshot: " << m_dumpPath << "\n";
            logFile << "====================================================\n\n";
            logFile.close();
        }

        // Serialize active graph to emergency JSON project file
        if (m_activeGraph) {
            std::filesystem::path dumpP(m_dumpPath);
            if (dumpP.has_parent_path() && !std::filesystem::exists(dumpP.parent_path())) {
                std::filesystem::create_directories(dumpP.parent_path());
            }

            nlohmann::json root;
            root["format_version"] = 1;
            root["generator"] = "NodeForge CrashReporter";
            root["crash_reason"] = reason;
            root["graph"] = GraphSerializer::Serialize(*m_activeGraph);

            std::ofstream snapFile(m_dumpPath);
            if (snapFile.is_open()) {
                snapFile << root.dump(2);
                snapFile.close();
                spdlog::critical("CrashReporter: Emergency graph snapshot written to {}", m_dumpPath);
                return true;
            } else {
                spdlog::error("CrashReporter: Failed to open snapshot file at {}", m_dumpPath);
            }
        } else {
            spdlog::warn("CrashReporter: No active graph set for snapshot");
        }
    } catch (const std::exception& e) {
        spdlog::error("CrashReporter failed to write emergency snapshot: {}", e.what());
    }

    return false;
}

bool CrashReporter::HasCrashSnapshot(const std::string& path) const {
    std::string checkPath = path.empty() ? m_dumpPath : path;
    return std::filesystem::exists(checkPath);
}

void CrashReporter::SetCustomCrashHook(std::function<void(const std::string&)> hook) {
    m_customHook = std::move(hook);
}

} // namespace nf
