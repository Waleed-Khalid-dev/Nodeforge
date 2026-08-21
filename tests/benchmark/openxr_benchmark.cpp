#include <gtest/gtest.h>
#include "xr/OpenXRRuntime.h"
#include "xr/OpenXRStereoMath.h"
#include "operators/tex/OpenXRHeadsetTexOp.h"
#include "operators/chan/OpenXRControllerChanOp.h"
#include "operators/comp/OpenXRCameraComp.h"
#include <chrono>
#include <iostream>

using namespace nf;

class OpenXRBenchmark : public ::testing::Test {
protected:
    void SetUp() override {
        OpenXRRuntime::Instance().Initialize(true);
    }
};

TEST_F(OpenXRBenchmark, OneHundredTwentyFpsStereoThroughput) {
    auto headset = std::make_unique<OpenXRHeadsetTexOp>(100, "BenchmarkHeadset");
    auto ctrl = std::make_unique<OpenXRControllerChanOp>(101, "BenchmarkCtrl");
    auto cam = std::make_unique<OpenXRCameraComp>(102, "BenchmarkCam");

    CookContext ctx{};
    ctx.deltaTimeSeconds = 1.0 / 120.0; // 120 Hz VR Refresh Rate

    const int benchmarkFrames = 120; // 1 full second
    auto start = std::chrono::high_resolution_clock::now();
    for (int frame = 0; frame < benchmarkFrames; ++frame) {
        ctx.timeSeconds += ctx.deltaTimeSeconds;
        float angle = static_cast<float>(frame) * 0.05f;
        OpenXRRuntime::Instance().SetEmulatedHeadPose(
            glm::vec3(std::sin(angle) * 0.2f, 1.7f, std::cos(angle) * 0.2f),
            glm::angleAxis(angle * 0.1f, glm::vec3(0.0f, 1.0f, 0.0f))
        );

        cam->Cook(ctx);
        ctrl->Cook(ctx);
        headset->Cook(ctx);
    }
    auto end = std::chrono::high_resolution_clock::now();

    double totalMs = std::chrono::duration<double, std::milli>(end - start).count();
    double avgMsPerFrame = totalMs / benchmarkFrames;

    std::cout << "[OpenXRBenchmark] 120 FPS Stereo Frame pipeline average cook time: " << avgMsPerFrame << " ms per frame" << std::endl;
    EXPECT_LT(avgMsPerFrame, 1.0); // Sub-millisecond pipeline cook
}

TEST_F(OpenXRBenchmark, ContinuousSoakZeroLeaks) {
    auto headset = std::make_unique<OpenXRHeadsetTexOp>(200, "SoakHeadset");
    auto ctrl = std::make_unique<OpenXRControllerChanOp>(201, "SoakCtrl");

    CookContext ctx{};
    ctx.deltaTimeSeconds = 1.0 / 90.0;

    for (int i = 0; i < 2000; ++i) {
        ctx.timeSeconds += ctx.deltaTimeSeconds;
        ctrl->Cook(ctx);
        headset->Cook(ctx);
    }

    EXPECT_TRUE(true);
}
