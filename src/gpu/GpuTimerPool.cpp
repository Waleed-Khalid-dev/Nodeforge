#include "GpuTimerPool.h"
#include "gpu/Device.h"
#include <spdlog/spdlog.h>
#include <algorithm>

namespace nf {

GpuTimerPool& GpuTimerPool::Instance() {
    static GpuTimerPool s_instance;
    return s_instance;
}

GpuTimerPool::GpuTimerPool() = default;

GpuTimerPool::~GpuTimerPool() {
    Shutdown();
}

bool GpuTimerPool::Initialize(gpu::Device* device, uint32_t maxQueries) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_device = device;
    m_maxQueries = maxQueries;

    if (!m_device) {
        spdlog::warn("GpuTimerPool initialized without GPU Device (mock mode active)");
        m_initialized = true;
        return true;
    }

    m_vkDevice = m_device->GetDevice();
    if (m_vkDevice == VK_NULL_HANDLE) {
        m_initialized = true;
        return true;
    }

    VkQueryPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
    poolInfo.queryType = VK_QUERY_TYPE_TIMESTAMP;
    poolInfo.queryCount = m_maxQueries;

    if (vkCreateQueryPool(m_vkDevice, &poolInfo, nullptr, &m_queryPool) != VK_SUCCESS) {
        spdlog::error("Failed to create Vulkan timestamp query pool");
        return false;
    }

    // Retrieve timestamp period from device properties if available (default ~1.0ns on modern GPUs)
    m_timestampPeriod = 1.0f;

    m_initialized = true;
    spdlog::info("GpuTimerPool initialized successfully with {} queries", m_maxQueries);
    return true;
}

void GpuTimerPool::Shutdown() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_vkDevice && m_queryPool) {
        vkDestroyQueryPool(m_vkDevice, m_queryPool, nullptr);
        m_queryPool = VK_NULL_HANDLE;
    }
    m_initialized = false;
    m_activeTimers.clear();
    m_latestDurations.clear();
}

void GpuTimerPool::BeginFrame() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_currentQueryIndex = 0;
    m_activeTimers.clear();

    if (m_vkDevice && m_queryPool) {
        vkResetQueryPool(m_vkDevice, m_queryPool, 0, m_maxQueries);
    }
}

uint32_t GpuTimerPool::StartTimer(VkCommandBuffer cmd, const std::string& passName) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_initialized || m_currentQueryIndex + 2 > m_maxQueries) {
        return 0xFFFFFFFF;
    }

    uint32_t timerId = static_cast<uint32_t>(m_activeTimers.size());
    GpuPassTiming timing;
    timing.name = passName;
    timing.startQueryIndex = m_currentQueryIndex++;
    timing.endQueryIndex = 0;
    timing.valid = false;

    if (cmd && m_queryPool) {
        vkCmdWriteTimestamp(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, m_queryPool, timing.startQueryIndex);
    }

    m_activeTimers.push_back(timing);
    return timerId;
}

void GpuTimerPool::StopTimer(VkCommandBuffer cmd, uint32_t timerId) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_initialized || timerId >= m_activeTimers.size() || m_currentQueryIndex >= m_maxQueries) {
        return;
    }

    auto& timing = m_activeTimers[timerId];
    timing.endQueryIndex = m_currentQueryIndex++;
    timing.valid = true;

    if (cmd && m_queryPool) {
        vkCmdWriteTimestamp(cmd, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, m_queryPool, timing.endQueryIndex);
    }
}

void GpuTimerPool::EndFrame() {
    // Frame commands submitted
}

void GpuTimerPool::ResolveResults() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_initialized || m_activeTimers.empty()) return;

    if (m_vkDevice && m_queryPool && m_currentQueryIndex > 0) {
        std::vector<uint64_t> queryResults(m_currentQueryIndex);
        VkResult res = vkGetQueryPoolResults(
            m_vkDevice, m_queryPool, 0, m_currentQueryIndex,
            queryResults.size() * sizeof(uint64_t), queryResults.data(),
            sizeof(uint64_t), VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WAIT_BIT
        );

        if (res == VK_SUCCESS) {
            double total = 0.0;
            for (auto& timing : m_activeTimers) {
                if (timing.valid && timing.endQueryIndex < queryResults.size()) {
                    uint64_t start = queryResults[timing.startQueryIndex];
                    uint64_t end = queryResults[timing.endQueryIndex];
                    if (end >= start) {
                        timing.durationMs = (static_cast<double>(end - start) * static_cast<double>(m_timestampPeriod)) / 1000000.0;
                    } else {
                        timing.durationMs = 0.0;
                    }
                    m_latestDurations[timing.name] = timing.durationMs;
                    total += timing.durationMs;
                }
            }
            m_totalGpuTimeMs = total;
        }
    }
}

double GpuTimerPool::GetPassDurationMs(const std::string& passName) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_latestDurations.find(passName);
    return (it != m_latestDurations.end()) ? it->second : 0.0;
}

double GpuTimerPool::GetTotalGpuTimeMs() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_totalGpuTimeMs;
}

std::vector<GpuPassTiming> GpuTimerPool::GetAllPassTimings() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<GpuPassTiming> results;
    for (const auto& [name, duration] : m_latestDurations) {
        GpuPassTiming t;
        t.name = name;
        t.durationMs = duration;
        t.valid = true;
        results.push_back(t);
    }
    return results;
}

void GpuTimerPool::InjectMockPassDuration(const std::string& passName, double durationMs) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_latestDurations[passName] = durationMs;
    m_totalGpuTimeMs = 0.0;
    for (const auto& [n, d] : m_latestDurations) {
        m_totalGpuTimeMs += d;
    }
}

void GpuTimerPool::Clear() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_latestDurations.clear();
    m_totalGpuTimeMs = 0.0;
}

} // namespace nf
