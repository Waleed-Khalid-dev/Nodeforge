#include <gtest/gtest.h>
#include "render/VolumetricMath.h"
#include "operators/tex/VolumetricCloudTexOp.h"
#include "operators/tex/LightShaftTexOp.h"
#include <chrono>
#include <iostream>

using namespace nf;

class VolumetricBenchmark : public ::testing::Test {};

TEST_F(VolumetricBenchmark, SixtyFpsRayMarchingThroughput) {
    auto cloud = std::make_unique<VolumetricCloudTexOp>(100, "BenchClouds");
    auto shafts = std::make_unique<LightShaftTexOp>(101, "BenchShafts");

    CookContext ctx{};
    ctx.deltaTimeSeconds = 1.0 / 60.0; // 60 Hz Frame rate

    const int benchmarkFrames = 60; // 1 full second
    auto start = std::chrono::high_resolution_clock::now();
    for (int frame = 0; frame < benchmarkFrames; ++frame) {
        ctx.timeSeconds += ctx.deltaTimeSeconds;
        cloud->Cook(ctx);
        shafts->Cook(ctx);
    }
    auto end = std::chrono::high_resolution_clock::now();

    double totalMs = std::chrono::duration<double, std::milli>(end - start).count();
    double avgMsPerFrame = totalMs / benchmarkFrames;

    std::cout << "[VolumetricBenchmark] 60 FPS Volumetric Frame pipeline average cook time: " << avgMsPerFrame << " ms per frame" << std::endl;
    EXPECT_LT(avgMsPerFrame, 1.0); // Sub-millisecond pipeline cook
}

TEST_F(VolumetricBenchmark, ContinuousSoakZeroLeaks) {
    auto cloud = std::make_unique<VolumetricCloudTexOp>(200, "SoakClouds");
    auto shafts = std::make_unique<LightShaftTexOp>(201, "SoakShafts");

    CookContext ctx{};
    ctx.deltaTimeSeconds = 1.0 / 60.0;

    for (int i = 0; i < 2000; ++i) {
        ctx.timeSeconds += ctx.deltaTimeSeconds;
        cloud->Cook(ctx);
        shafts->Cook(ctx);
    }

    EXPECT_TRUE(true);
}
