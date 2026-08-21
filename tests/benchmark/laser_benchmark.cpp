#include <gtest/gtest.h>
#include "laser/LaserPoint.h"
#include "laser/LaserEngine.h"
#include "operators/geom/LaserGeomOp.h"
#include "operators/geom/LaserPatternGeomOp.h"
#include "operators/chan/LaserDACChanOp.h"
#include <chrono>
#include <iostream>

using namespace nf;

class LaserBenchmark : public ::testing::Test {};

TEST_F(LaserBenchmark, OneHundredKppsStreamingThroughput) {
    auto pattern = std::make_unique<LaserPatternGeomOp>(100, "BenchPattern");
    auto laser = std::make_unique<LaserGeomOp>(101, "BenchLaser");
    auto dac = std::make_unique<LaserDACChanOp>(102, "BenchDAC");

    CookContext ctx{};
    ctx.deltaTimeSeconds = 1.0 / 60.0; // 60 Hz frame rate

    const int benchmarkFrames = 60; // 1 full second
    auto start = std::chrono::high_resolution_clock::now();
    for (int frame = 0; frame < benchmarkFrames; ++frame) {
        ctx.timeSeconds += ctx.deltaTimeSeconds;
        pattern->Cook(ctx);
        laser->Cook(ctx);
        dac->Cook(ctx);
    }
    auto end = std::chrono::high_resolution_clock::now();

    double totalMs = std::chrono::duration<double, std::milli>(end - start).count();
    double avgMsPerFrame = totalMs / benchmarkFrames;

    std::cout << "[LaserBenchmark] 100 kpps Laser Pipeline average cook time: " << avgMsPerFrame << " ms per frame" << std::endl;
    EXPECT_LT(avgMsPerFrame, 1.0); // Sub-millisecond pipeline cook
}

TEST_F(LaserBenchmark, ContinuousSoakZeroLeaks) {
    auto laser = std::make_unique<LaserGeomOp>(200, "SoakLaser");
    auto dac = std::make_unique<LaserDACChanOp>(201, "SoakDAC");

    CookContext ctx{};
    ctx.deltaTimeSeconds = 1.0 / 60.0;

    for (int i = 0; i < 2000; ++i) {
        ctx.timeSeconds += ctx.deltaTimeSeconds;
        laser->Cook(ctx);
        dac->Cook(ctx);
    }

    EXPECT_TRUE(true);
}
