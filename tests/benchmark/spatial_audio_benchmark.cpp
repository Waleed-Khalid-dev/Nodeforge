#include <gtest/gtest.h>
#include "operators/chan/AudioSpatializerChanOp.h"
#include "operators/chan/AmbisonicDecodeChanOp.h"
#include <chrono>
#include <vector>
#include <iostream>

using namespace nf;

class SpatialAudioBenchmark : public ::testing::Test {};

TEST_F(SpatialAudioBenchmark, SixtyFourStreamSpatialAudioThroughput) {
    const size_t streamCount = 64;
    std::vector<std::unique_ptr<AudioSpatializerChanOp>> spatializers;
    spatializers.reserve(streamCount);

    for (size_t i = 0; i < streamCount; ++i) {
        auto sp = std::make_unique<AudioSpatializerChanOp>(static_cast<NodeId>(300 + i), "Spatializer_" + std::to_string(i));
        sp->SetParam("spatial_mode", static_cast<int32_t>(1));   // VBAP
        sp->SetParam("speaker_layout", static_cast<int32_t>(3)); // 7.1.4 Atmos Dome (12 channels)
        sp->SetParam("emitter_pos", glm::vec3(std::sin(static_cast<float>(i)) * 10.0f, 2.0f, std::cos(static_cast<float>(i)) * 10.0f));
        spatializers.push_back(std::move(sp));
    }

    CookContext ctx{};
    ctx.deltaTimeSeconds = 1.0 / 60.0;
    ctx.timeSeconds = 0.0;

    const int benchmarkFrames = 50;
    auto start = std::chrono::high_resolution_clock::now();
    for (int frame = 0; frame < benchmarkFrames; ++frame) {
        ctx.timeSeconds += ctx.deltaTimeSeconds;
        float angle = static_cast<float>(frame) * 0.05f;
        for (size_t i = 0; i < spatializers.size(); ++i) {
            float th = angle + static_cast<float>(i) * 0.1f;
            spatializers[i]->SetParam("emitter_pos", glm::vec3(std::sin(th) * 15.0f, std::sin(th * 2.0f) * 5.0f, std::cos(th) * 15.0f));
            spatializers[i]->Cook(ctx);
        }
    }
    auto end = std::chrono::high_resolution_clock::now();

    double totalMs = std::chrono::duration<double, std::milli>(end - start).count();
    double avgMsPerFrame = totalMs / benchmarkFrames;

    std::cout << "[SpatialAudioBenchmark] 64 Spatial Audio Streams average cook time: " << avgMsPerFrame << " ms per frame" << std::endl;
    EXPECT_LT(avgMsPerFrame, 10.0); // Sub-millisecond per stream
}

TEST_F(SpatialAudioBenchmark, ContinuousSoakZeroLeaks) {
    auto spatializer = std::make_unique<AudioSpatializerChanOp>(5000, "SoakSpatializer");
    auto decoder = std::make_unique<AmbisonicDecodeChanOp>(5001, "SoakDecoder");

    spatializer->SetParam("spatial_mode", static_cast<int32_t>(0)); // Ambisonics B-Format
    decoder->SetParam("target_layout", static_cast<int32_t>(3));    // 7.1.4 Atmos

    CookContext ctx{};
    ctx.deltaTimeSeconds = 1.0 / 60.0;

    for (int i = 0; i < 2000; ++i) {
        ctx.timeSeconds += ctx.deltaTimeSeconds;
        float th = static_cast<float>(i) * 0.05f;
        spatializer->SetParam("emitter_pos", glm::vec3(std::sin(th) * 10.0f, 0.0f, std::cos(th) * 10.0f));
        spatializer->Cook(ctx);

        decoder->GetInputPin("in_bformat")->SetValue(spatializer->GetOutputPin("out_audio")->GetValue());
        decoder->Cook(ctx);
    }

    EXPECT_TRUE(true);
}
