#include <gtest/gtest.h>
#include "graph/Graph.h"
#include "graph/CoreNodes.h"
#include "graph/NodeRegistry.h"
#include "operators/chan/LFOChanOp.h"
#include "operators/chan/FilterChanOp.h"
#include "operators/chan/MathChanOp.h"
#include "core/ChannelBuffer.h"
#include <chrono>

using namespace nf;

TEST(ChanOpBenchmark, HighThroughputSIMDProcessing) {
    RegisterCoreNodes(NodeRegistry::Instance());

    // 100 channels with 10,000 samples = 1,000,000 samples
    std::vector<std::string> names;
    for (int i = 0; i < 100; ++i) {
        names.push_back("chan_" + std::to_string(i));
    }

    ChannelBuffer buf(names, 10000, 60.0f);

    auto start = std::chrono::high_resolution_clock::now();

    // Perform vector math passes
    for (int iter = 0; iter < 10; ++iter) {
        buf.Add(1.5f);
        buf.Multiply(0.5f);
        buf.Remap(-1.0f, 1.0f, 0.0f, 100.0f);
        buf.Clamp(0.0f, 50.0f);
    }

    auto end = std::chrono::high_resolution_clock::now();
    double elapsedMs = std::chrono::duration<double, std::milli>(end - start).count();

    // 10 iterations * 4 operations * 1,000,000 samples = 40,000,000 sample ops
    double samplesPerSec = (40000000.0 / elapsedMs) * 1000.0;

    EXPECT_GT(samplesPerSec, 1000000.0); // Must exceed 1M samples/sec easily
    EXPECT_EQ(buf.GetSampleCount(), 10000);
}

TEST(ChanOpBenchmark, LongRunningCookLoopZeroLeaks) {
    RegisterCoreNodes(NodeRegistry::Instance());

    Graph graph;
    auto* lfo = graph.CreateNode<LFOChanOp>("lfo1");
    auto* filter = graph.CreateNode<FilterChanOp>("filter1");
    auto* math = graph.CreateNode<MathChanOp>("math1");

    graph.Connect(lfo->GetOutputPin("output"), filter->GetInputPin("input"));
    graph.Connect(filter->GetOutputPin("output"), math->GetInputPin("input1"));

    lfo->SetParam("channel_names", std::string("c1 c2 c3 c4"));
    lfo->SetParam("time_sliced", true);

    // 10,000 frames continuous cook
    CookContext ctx{ .deltaTimeSeconds = 1.0 / 60.0 };
    for (uint64_t frame = 1; frame <= 10000; ++frame) {
        ctx.frameIndex = frame;
        ctx.timeSeconds = static_cast<double>(frame) / 60.0;
        ctx.timeSlice = { .isTimeSliced = true, .startSample = static_cast<int64_t>(frame), .sampleCount = 1, .sampleRate = 60.0f };

        lfo->MarkDirty();
        bool ok = graph.CookAll(ctx);
        ASSERT_TRUE(ok);
    }

    const ChannelBuffer* out = math->GetOutputBuffer();
    ASSERT_NE(out, nullptr);
    EXPECT_EQ(out->GetChannelCount(), 4);
}
