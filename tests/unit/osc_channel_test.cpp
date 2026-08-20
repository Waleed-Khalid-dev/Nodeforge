#include <gtest/gtest.h>
#include "core/ChannelBuffer.h"
#include "graph/Graph.h"
#include "operators/chan/OSCInChanOp.h"
#include "operators/chan/OSCOutChanOp.h"
#include "operators/chan/ConstantChanOp.h"

using namespace nf;

TEST(OSCChannelTest, OSCInChanOpMockInjectionAndAddressMapping) {
    OSCInChanOp oscIn(1, "osc_in1");
    oscIn.SetParam("active", false); // Do not open real network port in unit test

    // Inject single float address /speed -> 0.75
    oscIn.InjectMockOSC("/speed", { 0.75f });

    // Inject multi-float address /pos -> [1.0, 2.5, 3.2]
    oscIn.InjectMockOSC("/pos", { 1.0f, 2.5f, 3.2f });

    // Inject nested address /light/color -> [0.8, 0.2, 0.9]
    oscIn.InjectMockOSC("/light/color", { 0.8f, 0.2f, 0.9f });

    CookContext ctx;
    EXPECT_TRUE(oscIn.Cook(ctx));

    Pin* outPin = oscIn.GetOutputPin("output");
    ASSERT_NE(outPin, nullptr);
    ChannelBuffer buf = outPin->GetValue().Get<ChannelBuffer>();

    EXPECT_TRUE(buf.HasChannel("speed"));
    EXPECT_FLOAT_EQ(*buf.GetChannelData("speed"), 0.75f);

    EXPECT_TRUE(buf.HasChannel("pos_1"));
    EXPECT_TRUE(buf.HasChannel("pos_2"));
    EXPECT_TRUE(buf.HasChannel("pos_3"));
    EXPECT_FLOAT_EQ(*buf.GetChannelData("pos_1"), 1.0f);
    EXPECT_FLOAT_EQ(*buf.GetChannelData("pos_2"), 2.5f);
    EXPECT_FLOAT_EQ(*buf.GetChannelData("pos_3"), 3.2f);

    EXPECT_TRUE(buf.HasChannel("light_color_1"));
    EXPECT_FLOAT_EQ(*buf.GetChannelData("light_color_1"), 0.8f);
}

TEST(OSCChannelTest, OSCInChanOpDecayRate) {
    OSCInChanOp oscIn(2, "osc_in2");
    oscIn.SetParam("active", false);
    oscIn.SetParam("decay_rate", 6.0f); // 10% decay per cook

    oscIn.InjectMockOSC("/pulse", { 1.0f });

    CookContext ctx;
    oscIn.Cook(ctx);
    Pin* outPin = oscIn.GetOutputPin("output");
    ASSERT_NE(outPin, nullptr);
    ChannelBuffer buf1 = outPin->GetValue().Get<ChannelBuffer>();
    float val1 = *buf1.GetChannelData("pulse");
    EXPECT_FLOAT_EQ(val1, 1.0f);

    // Second cook should decay
    oscIn.Cook(ctx);
    ChannelBuffer buf2 = outPin->GetValue().Get<ChannelBuffer>();
    float val2 = *buf2.GetChannelData("pulse");
    EXPECT_LT(val2, val1);
}

TEST(OSCChannelTest, OSCOutChanOpGraphPipeline) {
    Graph graph;
    auto* constNode = graph.CreateNode<ConstantChanOp>("const1");
    auto* oscOut = graph.CreateNode<OSCOutChanOp>("osc_out1");

    constNode->SetParam("channel_names", std::string("fader1 fader2"));
    constNode->SetParam("values", glm::vec4(0.42f, 0.88f, 0.0f, 0.0f));

    auto* wire = graph.Connect(constNode->GetOutputPin("output"), oscOut->GetInputPin("input"));
    ASSERT_NE(wire, nullptr);

    CookContext ctx;
    EXPECT_TRUE(graph.CookNode(oscOut, ctx));

    Pin* outPin = oscOut->GetOutputPin("output");
    ASSERT_NE(outPin, nullptr);
    ChannelBuffer outBuf = outPin->GetValue().Get<ChannelBuffer>();
    EXPECT_EQ(outBuf.GetChannelCount(), 2u);
    EXPECT_FLOAT_EQ(*outBuf.GetChannelData("fader1"), 0.42f);
    EXPECT_FLOAT_EQ(*outBuf.GetChannelData("fader2"), 0.88f);
}
