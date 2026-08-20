#include <gtest/gtest.h>
#include "profiling/CookProfiler.h"
#include "gpu/GpuTimerPool.h"
#include <thread>
#include <chrono>

using namespace nf;

TEST(ProfilerTest, CookProfilerRecordAndStatistics) {
    auto& profiler = CookProfiler::Instance();
    profiler.ResetAll();

    profiler.BeginFrame(1, 1.0 / 60.0);

    // Record sample cook times for node 42
    profiler.RecordNodeCook(42, "blur1", "BlurTexOp", "TexOp", 2.5);
    profiler.RecordNodeCook(42, "blur1", "BlurTexOp", "TexOp", 3.5);
    profiler.RecordNodeCook(42, "blur1", "BlurTexOp", "TexOp", 1.5);

    profiler.EndFrame();

    NodePerfStats stats;
    EXPECT_TRUE(profiler.GetNodeStats(42, stats));
    EXPECT_EQ(stats.id, 42u);
    EXPECT_EQ(stats.name, "blur1");
    EXPECT_EQ(stats.type, "BlurTexOp");
    EXPECT_EQ(stats.totalCooks, 3u);
    EXPECT_FLOAT_EQ(static_cast<float>(stats.lastCpuMs), 1.5f);
    EXPECT_FLOAT_EQ(static_cast<float>(stats.minCpuMs), 1.5f);
    EXPECT_FLOAT_EQ(static_cast<float>(stats.maxCpuMs), 3.5f);

    auto frameData = profiler.GetLatestFrameData();
    EXPECT_EQ(frameData.frameIndex, 1u);
    EXPECT_GT(frameData.currentFps, 0.0f);
}

TEST(ProfilerTest, ScopedNodeTimerRAII) {
    auto& profiler = CookProfiler::Instance();
    profiler.ResetAll();

    {
        ScopedNodeTimer timer(99, "math1", "MathChanOp", "ChanOp");
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    NodePerfStats stats;
    EXPECT_TRUE(profiler.GetNodeStats(99, stats));
    EXPECT_EQ(stats.name, "math1");
    EXPECT_GE(stats.lastCpuMs, 3.0); // Must measure at least ~3-5ms
}

TEST(ProfilerTest, GpuTimerPoolPassRecording) {
    auto& gpuTimer = GpuTimerPool::Instance();
    gpuTimer.Clear();

    gpuTimer.InjectMockPassDuration("BlurPass_Horizontal", 0.45);
    gpuTimer.InjectMockPassDuration("BlurPass_Vertical", 0.42);
    gpuTimer.InjectMockPassDuration("CompositePass", 0.15);

    EXPECT_NEAR(gpuTimer.GetPassDurationMs("BlurPass_Horizontal"), 0.45, 0.001);
    EXPECT_NEAR(gpuTimer.GetPassDurationMs("BlurPass_Vertical"), 0.42, 0.001);
    EXPECT_NEAR(gpuTimer.GetTotalGpuTimeMs(), 1.02, 0.001);

    auto all = gpuTimer.GetAllPassTimings();
    EXPECT_EQ(all.size(), 3u);
}
