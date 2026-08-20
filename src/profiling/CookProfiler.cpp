#include "CookProfiler.h"
#include <algorithm>

namespace nf {

CookProfiler& CookProfiler::Instance() {
    static CookProfiler s_instance;
    return s_instance;
}

CookProfiler::CookProfiler() {
    m_fpsHistory.fill(60.0f);
    m_frameTimeHistory.fill(16.666f);
    m_lastFrameTime = std::chrono::high_resolution_clock::now();
}

void CookProfiler::BeginFrame(uint64_t frameIndex, double deltaSeconds) {
    if (!m_enabled) return;

    std::lock_guard<std::mutex> lock(m_mutex);
    m_currentFrame = frameIndex;
    m_frameStartTime = std::chrono::high_resolution_clock::now();

    double deltaMs = deltaSeconds * 1000.0;
    if (deltaMs <= 0.0) deltaMs = 16.666;

    float fps = static_cast<float>(1.0 / (deltaSeconds > 0.0001 ? deltaSeconds : 0.016666));
    fps = std::clamp(fps, 0.0f, 999.0f);

    m_latestFrame.frameIndex = frameIndex;
    m_latestFrame.frameDeltaMs = deltaMs;
    m_latestFrame.currentFps = fps;
    m_latestFrame.totalGraphCpuMs = 0.0;
    m_latestFrame.totalGpuMs = 0.0;

    m_fpsHistory[m_historyHead] = fps;
    m_frameTimeHistory[m_historyHead] = static_cast<float>(deltaMs);
    m_historyHead = (m_historyHead + 1) % m_fpsHistory.size();
}

void CookProfiler::EndFrame() {
    if (!m_enabled) return;

    std::lock_guard<std::mutex> lock(m_mutex);
    auto now = std::chrono::high_resolution_clock::now();
    double totalMs = std::chrono::duration<double, std::milli>(now - m_frameStartTime).count();
    m_latestFrame.totalGraphCpuMs = totalMs;
}

void CookProfiler::RecordNodeCook(NodeId id, const std::string& name, const std::string& type, const std::string& family, double cpuDurationMs, double gpuDurationMs) {
    if (!m_enabled) return;

    std::lock_guard<std::mutex> lock(m_mutex);
    auto& stat = m_nodeStats[id];
    stat.id = id;
    stat.name = name;
    stat.type = type;
    stat.family = family;
    stat.lastCpuMs = cpuDurationMs;
    stat.lastGpuMs = gpuDurationMs;
    stat.minCpuMs = std::min(stat.minCpuMs, cpuDurationMs);
    stat.maxCpuMs = std::max(stat.maxCpuMs, cpuDurationMs);
    stat.totalCooks++;

    // Rolling exponential average
    if (stat.totalCooks == 1) {
        stat.avgCpuMs = cpuDurationMs;
    } else {
        stat.avgCpuMs = (stat.avgCpuMs * 0.9) + (cpuDurationMs * 0.1);
    }

    stat.history[stat.historyIndex] = static_cast<float>(cpuDurationMs);
    stat.historyIndex = (stat.historyIndex + 1) % stat.history.size();
}

std::vector<NodePerfStats> CookProfiler::GetAllNodeStats() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<NodePerfStats> results;
    results.reserve(m_nodeStats.size());
    for (const auto& [id, stat] : m_nodeStats) {
        results.push_back(stat);
    }
    return results;
}

bool CookProfiler::GetNodeStats(NodeId id, NodePerfStats& outStats) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_nodeStats.find(id);
    if (it != m_nodeStats.end()) {
        outStats = it->second;
        return true;
    }
    return false;
}

FrameProfileData CookProfiler::GetLatestFrameData() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_latestFrame;
}

const std::array<float, 120>& CookProfiler::GetFpsHistory() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_fpsHistory;
}

const std::array<float, 120>& CookProfiler::GetFrameTimeHistory() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_frameTimeHistory;
}

void CookProfiler::ResetAll() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_nodeStats.clear();
    m_fpsHistory.fill(60.0f);
    m_frameTimeHistory.fill(16.666f);
    m_historyHead = 0;
    m_latestFrame = FrameProfileData{};
}

} // namespace nf
