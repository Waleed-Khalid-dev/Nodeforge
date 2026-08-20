#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <vulkan/vulkan.h>

namespace gpu {
class Device;
}

namespace nf {

struct GpuPassTiming {
    std::string name;
    double durationMs{0.0};
    uint32_t startQueryIndex{0};
    uint32_t endQueryIndex{0};
    bool valid{false};
};

class GpuTimerPool {
public:
    static GpuTimerPool& Instance();

    GpuTimerPool();
    ~GpuTimerPool();

    bool Initialize(gpu::Device* device, uint32_t maxQueries = 256);
    void Shutdown();

    // Pass timing commands (recorded into command buffer)
    void BeginFrame();
    uint32_t StartTimer(VkCommandBuffer cmd, const std::string& passName);
    void StopTimer(VkCommandBuffer cmd, uint32_t timerId);
    void EndFrame();

    // Query resolution (called at beginning of next frame or after sync)
    void ResolveResults();

    // Retrieval
    double GetPassDurationMs(const std::string& passName) const;
    double GetTotalGpuTimeMs() const;
    std::vector<GpuPassTiming> GetAllPassTimings() const;

    // Simulation / testing injection
    void InjectMockPassDuration(const std::string& passName, double durationMs);
    void Clear();

private:
    gpu::Device* m_device{nullptr};
    VkDevice m_vkDevice{VK_NULL_HANDLE};
    VkQueryPool m_queryPool{VK_NULL_HANDLE};
    float m_timestampPeriod{1.0f}; // nanoseconds per tick
    uint32_t m_maxQueries{256};
    uint32_t m_currentQueryIndex{0};

    mutable std::mutex m_mutex;
    bool m_initialized{false};
    std::vector<GpuPassTiming> m_activeTimers;
    std::unordered_map<std::string, double> m_latestDurations;
    double m_totalGpuTimeMs{0.0};
};

} // namespace nf
