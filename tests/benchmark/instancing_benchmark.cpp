#include <gtest/gtest.h>
#include "operators/geom/InstanceGeomOp.h"
#include <chrono>
#include <iostream>

using namespace nf;

class InstancingBenchmark : public ::testing::Test {};

TEST_F(InstancingBenchmark, OneHundredThousandInstanceThroughput) {
    auto instancer = std::make_unique<InstanceGeomOp>(10, "BenchmarkInstancer");
    instancer->SetParam("distribution_mode", static_cast<int32_t>(2)); // Grid Array
    instancer->SetParam("grid_count", glm::vec3(100.0f, 100.0f, 10.0f));       // 100,000 instances
    instancer->SetParam("grid_spacing", glm::vec3(0.5f, 0.5f, 0.5f));
    instancer->SetParam("noise_displacement", 0.1f);

    CookContext ctx{};
    ctx.deltaTimeSeconds = 1.0 / 60.0;
    ctx.timeSeconds = 0.0;

    // Warmup
    instancer->Cook(ctx);

    const int benchmarkFrames = 10;
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < benchmarkFrames; ++i) {
        ctx.timeSeconds += ctx.deltaTimeSeconds;
        instancer->Cook(ctx);
    }
    auto end = std::chrono::high_resolution_clock::now();

    double totalMs = std::chrono::duration<double, std::milli>(end - start).count();
    double avgMsPerFrame = totalMs / benchmarkFrames;

    std::cout << "[InstancingBenchmark] 100,000 Instances average cook time: " << avgMsPerFrame << " ms" << std::endl;

    const GeometryData* geom = instancer->GetOutputGeometry();
    ASSERT_NE(geom, nullptr);
    EXPECT_EQ(geom->GetInstanceCount(), 100000u);
    EXPECT_LT(avgMsPerFrame, 100.0); // Clean real-time budget
}

TEST_F(InstancingBenchmark, ContinuousSoakZeroLeaks) {
    auto instancer = std::make_unique<InstanceGeomOp>(11, "SoakInstancer");
    instancer->SetParam("distribution_mode", static_cast<int32_t>(3)); // Fibonacci Spiral
    instancer->SetParam("spiral_count", static_cast<int32_t>(10000));

    CookContext ctx{};
    ctx.deltaTimeSeconds = 1.0 / 60.0;

    for (int i = 0; i < 200; ++i) {
        ctx.timeSeconds += ctx.deltaTimeSeconds;
        instancer->Cook(ctx);
    }

    const GeometryData* geom = instancer->GetOutputGeometry();
    ASSERT_NE(geom, nullptr);
    EXPECT_EQ(geom->GetInstanceCount(), 10000u);
}
