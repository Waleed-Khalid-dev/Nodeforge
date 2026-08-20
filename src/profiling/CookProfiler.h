#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <mutex>
#include <array>

namespace nf {

using NodeId = uint64_t;

struct NodePerfStats {
    NodeId id{0};
    std::string name;
    std::string type;
    std::string family;
    double lastCpuMs{0.0};
    double avgCpuMs{0.0};
    double minCpuMs{999999.0};
    double maxCpuMs{0.0};
    double lastGpuMs{0.0};
    uint64_t totalCooks{0};
    std::array<float, 60> history{};
    size_t historyIndex{0};
};

struct FrameProfileData {
    uint64_t frameIndex{0};
    double totalGraphCpuMs{0.0};
    double totalGpuMs{0.0};
    double frameDeltaMs{16.666};
    float currentFps{60.0f};
    size_t activeNodeCount{0};
    size_t dirtyNodeCount{0};
};

class CookProfiler {
public:
    static CookProfiler& Instance();

    CookProfiler();
    ~CookProfiler() = default;

    // Frame lifecycle
    void BeginFrame(uint64_t frameIndex, double deltaSeconds);
    void EndFrame();

    // Node cook recording
    void RecordNodeCook(NodeId id, const std::string& name, const std::string& type, const std::string& family, double cpuDurationMs, double gpuDurationMs = 0.0);

    // Queries for UI & diagnostics
    std::vector<NodePerfStats> GetAllNodeStats() const;
    bool GetNodeStats(NodeId id, NodePerfStats& outStats) const;
    FrameProfileData GetLatestFrameData() const;
    const std::array<float, 120>& GetFpsHistory() const;
    const std::array<float, 120>& GetFrameTimeHistory() const;

    // Reset & clear
    void ResetAll();
    void SetEnabled(bool enabled) { m_enabled = enabled; }
    bool IsEnabled() const { return m_enabled; }

private:
    mutable std::mutex m_mutex;
    bool m_enabled{true};
    uint64_t m_currentFrame{0};
    std::chrono::high_resolution_clock::time_point m_frameStartTime;
    std::chrono::high_resolution_clock::time_point m_lastFrameTime;

    FrameProfileData m_latestFrame;
    std::unordered_map<NodeId, NodePerfStats> m_nodeStats;

    std::array<float, 120> m_fpsHistory{};
    std::array<float, 120> m_frameTimeHistory{};
    size_t m_historyHead{0};
};

/// RAII Helper for timing a node cook execution
class ScopedNodeTimer {
public:
    ScopedNodeTimer(NodeId id, const std::string& name, const std::string& type, const std::string& family = "Operator")
        : m_id(id), m_name(name), m_type(type), m_family(family), m_start(std::chrono::high_resolution_clock::now()) {}

    ~ScopedNodeTimer() {
        auto end = std::chrono::high_resolution_clock::now();
        double ms = std::chrono::duration<double, std::milli>(end - m_start).count();
        CookProfiler::Instance().RecordNodeCook(m_id, m_name, m_type, m_family, ms);
    }

private:
    NodeId m_id;
    std::string m_name;
    std::string m_type;
    std::string m_family;
    std::chrono::high_resolution_clock::time_point m_start;
};

} // namespace nf
