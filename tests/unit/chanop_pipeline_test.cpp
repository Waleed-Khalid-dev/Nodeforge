#include <gtest/gtest.h>
#include "graph/Graph.h"
#include "graph/CoreNodes.h"
#include "graph/NodeRegistry.h"
#include "operators/chan/ConstantChanOp.h"
#include "operators/chan/TimeChanOp.h"
#include "operators/chan/LFOChanOp.h"
#include "operators/chan/NoiseChanOp.h"
#include "operators/chan/MathChanOp.h"
#include "operators/chan/FilterChanOp.h"
#include "operators/chan/MergeChanOp.h"
#include "operators/chan/SelectChanOp.h"
#include "operators/chan/TrailChanOp.h"
#include "operators/chan/AudioFileInChanOp.h"
#include "operators/chan/ChanToTexOp.h"
#include "operators/chan/TexToChanOp.h"
#include <cmath>

using namespace nf;

class ChanOpPipelineTest : public ::testing::Test {
protected:
    void SetUp() override {
        RegisterCoreNodes(NodeRegistry::Instance());
    }
};

TEST_F(ChanOpPipelineTest, ConstantChanOpCook) {
    ConstantChanOp constNode(1, "const1");
    constNode.SetParam("channel_names", std::string("tx ty tz"));
    constNode.SetParam("values", glm::vec4(10.0f, 20.0f, 30.0f, 0.0f));
    constNode.SetParam("sample_count", static_cast<int32_t>(5));

    CookContext ctx{ .frameIndex = 1 };
    EXPECT_TRUE(constNode.Cook(ctx));

    const ChannelBuffer* buf = constNode.GetOutputBuffer();
    ASSERT_NE(buf, nullptr);
    EXPECT_EQ(buf->GetChannelCount(), 3);
    EXPECT_EQ(buf->GetSampleCount(), 5);
    EXPECT_FLOAT_EQ(buf->GetSample("tx", 0), 10.0f);
    EXPECT_FLOAT_EQ(buf->GetSample("ty", 2), 20.0f);
    EXPECT_FLOAT_EQ(buf->GetSample("tz", 4), 30.0f);
}

TEST_F(ChanOpPipelineTest, TimeChanOpCook) {
    TimeChanOp timeNode(2, "time1");
    CookContext ctx{
        .frameIndex = 120,
        .timeSeconds = 2.0,
        .deltaTimeSeconds = 1.0 / 60.0,
        .timeSlice = { .isTimeSliced = true, .sampleCount = 1 }
    };

    EXPECT_TRUE(timeNode.Cook(ctx));
    const ChannelBuffer* buf = timeNode.GetOutputBuffer();
    ASSERT_NE(buf, nullptr);
    EXPECT_NEAR(buf->GetSample("seconds", 0), 2.0f, 1e-4f);
    EXPECT_FLOAT_EQ(buf->GetSample("frame", 0), 120.0f);
}

TEST_F(ChanOpPipelineTest, LFOChanOpWaveforms) {
    LFOChanOp lfo(3, "lfo1");
    lfo.SetParam("type", static_cast<int32_t>(0)); // Sine
    lfo.SetParam("frequency", 1.0f);
    lfo.SetParam("amplitude", 2.0f);
    lfo.SetParam("offset", 1.0f);
    lfo.SetParam("time_sliced", true);

    // At t = 0: offset + amp * sin(0) = 1.0
    CookContext ctx0{ .frameIndex = 1, .timeSeconds = 0.0, .deltaTimeSeconds = 1.0 / 60.0 };
    EXPECT_TRUE(lfo.Cook(ctx0));
    EXPECT_NEAR(lfo.GetSample("chan1", 0), 1.0f, 1e-4f);

    // At t = 0.25s (90 deg): offset + amp * sin(pi/2) = 1.0 + 2.0 * 1.0 = 3.0
    CookContext ctx90{ .frameIndex = 15, .timeSeconds = 0.25, .deltaTimeSeconds = 1.0 / 60.0 };
    EXPECT_TRUE(lfo.Cook(ctx90));
    EXPECT_NEAR(lfo.GetSample("chan1", 0), 3.0f, 1e-4f);
}

TEST_F(ChanOpPipelineTest, MathChanOpCombineAndUnary) {
    Graph graph;
    auto* c1 = graph.CreateNode<ConstantChanOp>("c1");
    auto* c2 = graph.CreateNode<ConstantChanOp>("c2");
    auto* math = graph.CreateNode<MathChanOp>("math1");

    c1->SetParam("channel_names", std::string("val"));
    c1->SetParam("values", glm::vec4(10.0f, 0.0f, 0.0f, 0.0f));

    c2->SetParam("channel_names", std::string("val"));
    c2->SetParam("values", glm::vec4(5.0f, 0.0f, 0.0f, 0.0f));

    graph.Connect(c1->GetOutputPin("output"), math->GetInputPin("input1"));
    graph.Connect(c2->GetOutputPin("output"), math->GetInputPin("input2"));

    // Multiply combine mode
    math->SetParam("operation", static_cast<int32_t>(6)); // Combine
    math->SetParam("combine_mode", static_cast<int32_t>(2)); // Multiply

    CookContext ctx{ .frameIndex = 1 };
    EXPECT_TRUE(graph.CookAll(ctx));

    const ChannelBuffer* out = math->GetOutputBuffer();
    ASSERT_NE(out, nullptr);
    EXPECT_FLOAT_EQ(out->GetSample("val", 0), 50.0f);
}

TEST_F(ChanOpPipelineTest, FilterChanOpLag) {
    FilterChanOp filter(4, "filter1");
    filter.SetParam("type", static_cast<int32_t>(0)); // Lag
    filter.SetParam("lag_up", 0.1f);
    filter.SetParam("lag_down", 0.1f);

    ChannelBuffer inBuf({ "val" }, 1, 60.0f);
    inBuf.SetSample(0, 0, 10.0f);
    filter.GetInputPin("input")->SetValue(PinValue(inBuf));

    CookContext ctx{ .frameIndex = 1, .deltaTimeSeconds = 1.0 / 60.0 };
    EXPECT_TRUE(filter.Cook(ctx));

    // Initial step initializes state
    EXPECT_FLOAT_EQ(filter.GetSample("val", 0), 10.0f);

    // Step input to 20.0f
    inBuf.SetSample(0, 0, 20.0f);
    filter.GetInputPin("input")->SetValue(PinValue(inBuf));

    ctx.frameIndex = 2;
    EXPECT_TRUE(filter.Cook(ctx));
    float val1 = filter.GetSample("val", 0);
    EXPECT_GT(val1, 10.0f);
    EXPECT_LT(val1, 20.0f);
}

TEST_F(ChanOpPipelineTest, MergeAndSelectChanOp) {
    Graph graph;
    auto* c1 = graph.CreateNode<ConstantChanOp>("c1");
    auto* c2 = graph.CreateNode<ConstantChanOp>("c2");
    auto* merge = graph.CreateNode<MergeChanOp>("merge1");
    auto* select = graph.CreateNode<SelectChanOp>("select1");

    c1->SetParam("channel_names", std::string("tx ty"));
    c1->SetParam("values", glm::vec4(1.0f, 2.0f, 0.0f, 0.0f));

    c2->SetParam("channel_names", std::string("tz r g b"));
    c2->SetParam("values", glm::vec4(3.0f, 0.5f, 0.6f, 0.7f));

    graph.Connect(c1->GetOutputPin("output"), merge->GetInputPin("input1"));
    graph.Connect(c2->GetOutputPin("output"), merge->GetInputPin("input2"));
    graph.Connect(merge->GetOutputPin("output"), select->GetInputPin("input"));

    // Select only 't*' channels and rename to 'pos_x pos_y pos_z'
    select->SetParam("channel_names", std::string("t*"));
    select->SetParam("rename_to", std::string("pos_x pos_y pos_z"));

    CookContext ctx{ .frameIndex = 1 };
    EXPECT_TRUE(graph.CookAll(ctx));

    const ChannelBuffer* out = select->GetOutputBuffer();
    ASSERT_NE(out, nullptr);
    EXPECT_EQ(out->GetChannelCount(), 3);
    EXPECT_EQ(out->GetChannelNames()[0], "pos_x");
    EXPECT_EQ(out->GetChannelNames()[1], "pos_y");
    EXPECT_EQ(out->GetChannelNames()[2], "pos_z");
    EXPECT_FLOAT_EQ(out->GetSample("pos_x", 0), 1.0f);
    EXPECT_FLOAT_EQ(out->GetSample("pos_y", 0), 2.0f);
    EXPECT_FLOAT_EQ(out->GetSample("pos_z", 0), 3.0f);
}

TEST_F(ChanOpPipelineTest, TrailChanOpRingBuffer) {
    TrailChanOp trail(5, "trail1");
    trail.SetParam("max_samples", static_cast<int32_t>(5));

    CookContext ctx{ .frameIndex = 1, .deltaTimeSeconds = 1.0 / 60.0 };

    for (int i = 1; i <= 5; ++i) {
        ChannelBuffer inBuf({ "val" }, 1, 60.0f);
        inBuf.SetSample(0, 0, static_cast<float>(i * 10));
        trail.GetInputPin("input")->SetValue(PinValue(inBuf));
        ctx.frameIndex = i;
        EXPECT_TRUE(trail.Cook(ctx));
    }

    const ChannelBuffer* buf = trail.GetOutputBuffer();
    ASSERT_NE(buf, nullptr);
    EXPECT_EQ(buf->GetSampleCount(), 5);
    EXPECT_FLOAT_EQ(buf->GetSample("val", 0), 10.0f);
    EXPECT_FLOAT_EQ(buf->GetSample("val", 4), 50.0f);
}

TEST_F(ChanOpPipelineTest, AudioFileInSynthesizer) {
    AudioFileInChanOp audioNode(6, "audio1");
    audioNode.SetParam("file_path", std::string("")); // Generates synthetic tone
    audioNode.SetParam("volume", 0.8f);

    CookContext ctx{ .frameIndex = 1, .deltaTimeSeconds = 1.0 / 60.0, .timeSlice = { .isTimeSliced = true } };
    EXPECT_TRUE(audioNode.Cook(ctx));

    const ChannelBuffer* buf = audioNode.GetOutputBuffer();
    ASSERT_NE(buf, nullptr);
    EXPECT_EQ(buf->GetChannelCount(), 2);
    EXPECT_GT(buf->GetSampleCount(), 0);
    EXPECT_FLOAT_EQ(buf->GetSampleRate(), 44100.0f);
}
