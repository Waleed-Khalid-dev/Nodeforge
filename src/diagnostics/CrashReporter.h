#pragma once

#include <string>
#include <functional>

namespace nf {

class Graph;

class CrashReporter {
public:
    static CrashReporter& Instance();

    CrashReporter();
    ~CrashReporter();

    void Install();
    void Uninstall();

    void SetActiveGraph(Graph* graph);
    void SetCrashDumpPath(const std::string& path);

    bool WriteEmergencyCrashSnapshot(const std::string& reason = "Unhandled Exception");
    bool HasCrashSnapshot(const std::string& path = "") const;

    // Simulation / testing
    void SetCustomCrashHook(std::function<void(const std::string&)> hook);

private:
    Graph* m_activeGraph{nullptr};
    std::string m_dumpPath{"nodeforge_emergency_crash.nfp"};
    std::string m_logPath{"nodeforge_crash.log"};
    bool m_installed{false};
    std::function<void(const std::string&)> m_customHook;
};

} // namespace nf
