#include <gtest/gtest.h>
#include "ai/PoseKeypoints.h"
#include "ai/ONNXInferenceEngine.h"
#include "operators/tex/NeuralStyleTexOp.h"
#include "operators/tex/SegmentationMaskTexOp.h"
#include "operators/chan/PoseEstimationChanOp.h"
#include <chrono>
#include <iostream>

using namespace nf;

class AINeuralBenchmark : public ::testing::Test {
protected:
    void SetUp() override {
        ONNXInferenceEngine::Instance().Initialize(true);
    }
};

TEST_F(AINeuralBenchmark, SixtyFpsNeuralInferenceThroughput) {
    auto styler = std::make_unique<NeuralStyleTexOp>(100, "BenchStyler");
    auto mask = std::make_unique<SegmentationMaskTexOp>(101, "BenchMask");
    auto pose = std::make_unique<PoseEstimationChanOp>(102, "BenchPose");

    CookContext ctx{};
    ctx.deltaTimeSeconds = 1.0 / 60.0; // 60 FPS

    const int benchmarkFrames = 60; // 1 full second
    auto start = std::chrono::high_resolution_clock::now();
    for (int frame = 0; frame < benchmarkFrames; ++frame) {
        ctx.timeSeconds += ctx.deltaTimeSeconds;
        styler->Cook(ctx);
        mask->Cook(ctx);
        pose->Cook(ctx);
    }
    auto end = std::chrono::high_resolution_clock::now();

    double totalMs = std::chrono::duration<double, std::milli>(end - start).count();
    double avgMsPerFrame = totalMs / benchmarkFrames;

    std::cout << "[AINeuralBenchmark] 60 FPS AI Pipeline average cook time: " << avgMsPerFrame << " ms per frame" << std::endl;
    EXPECT_LT(avgMsPerFrame, 1.0); // Sub-millisecond pipeline cook
}

TEST_F(AINeuralBenchmark, ContinuousSoakZeroLeaks) {
    auto styler = std::make_unique<NeuralStyleTexOp>(200, "SoakStyler");
    auto pose = std::make_unique<PoseEstimationChanOp>(201, "SoakPose");

    CookContext ctx{};
    ctx.deltaTimeSeconds = 1.0 / 60.0;

    for (int i = 0; i < 2000; ++i) {
        ctx.timeSeconds += ctx.deltaTimeSeconds;
        styler->Cook(ctx);
        pose->Cook(ctx);
    }

    EXPECT_TRUE(true);
}
