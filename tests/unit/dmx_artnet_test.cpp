#include <gtest/gtest.h>
#include "core/ChannelBuffer.h"
#include "io/dmx/ArtNetEngine.h"
#include "operators/chan/DMXInChanOp.h"
#include "operators/chan/DMXOutChanOp.h"
#include "operators/chan/ConstantChanOp.h"
#include "graph/Graph.h"

using namespace nf;

TEST(DmxArtNetTest, ArtNetEngineMockUniverseInjection) {
    auto& engine = ArtNetEngine::Instance();
    engine.ClearUniverses();

    uint8_t dmxData[512] = {0};
    dmxData[0] = 255; // Channel 1 = 100%
    dmxData[1] = 128; // Channel 2 = ~50%
    dmxData[2] = 0;   // Channel 3 = 0%
    dmxData[511] = 255;

    engine.InjectMockDMX(0, dmxData, 512);

    float channels[512] = {0};
    EXPECT_TRUE(engine.GetUniverseChannels(0, channels, 512, true));

    EXPECT_FLOAT_EQ(channels[0], 1.0f);
    EXPECT_NEAR(channels[1], 128.0f / 255.0f, 0.01f);
    EXPECT_FLOAT_EQ(channels[2], 0.0f);
    EXPECT_FLOAT_EQ(channels[511], 1.0f);
}

TEST(DmxArtNetTest, DMXInChanOpCook) {
    auto& engine = ArtNetEngine::Instance();
    engine.ClearUniverses();

    uint8_t testDmx[512] = {0};
    testDmx[0] = 200;
    testDmx[9] = 150;
    engine.InjectMockDMX(1, testDmx, 512);

    DMXInChanOp dmxIn(1, "dmx_in1");
    dmxIn.SetParam("universe", static_cast<int32_t>(1));
    dmxIn.SetParam("num_channels", static_cast<int32_t>(16));
    dmxIn.SetParam("normalized", true);
    dmxIn.SetParam("active", true);

    CookContext ctx;
    EXPECT_TRUE(dmxIn.Cook(ctx));

    Pin* outPin = dmxIn.GetOutputPin("output");
    ASSERT_NE(outPin, nullptr);
    ChannelBuffer buf = outPin->GetValue().Get<ChannelBuffer>();

    EXPECT_EQ(buf.GetChannelCount(), 16u);
    EXPECT_TRUE(buf.HasChannel("ch1"));
    EXPECT_TRUE(buf.HasChannel("ch10"));

    EXPECT_NEAR(*buf.GetChannelData("ch1"), 200.0f / 255.0f, 0.01f);
    EXPECT_NEAR(*buf.GetChannelData("ch10"), 150.0f / 255.0f, 0.01f);
}

TEST(DmxArtNetTest, DMXOutChanOpPipeline) {
    Graph graph;
    auto* constOp = graph.CreateNode<ConstantChanOp>("lighting_src");
    auto* dmxOut = graph.CreateNode<DMXOutChanOp>("dmx_out1");

    constOp->SetParam("channel_names", std::string("dimmer red green blue"));
    constOp->SetParam("values", glm::vec4(1.0f, 0.8f, 0.2f, 0.0f));

    auto* wire = graph.Connect(constOp->GetOutputPin("output"), dmxOut->GetInputPin("input"));
    ASSERT_NE(wire, nullptr);

    CookContext ctx;
    EXPECT_TRUE(graph.CookNode(dmxOut, ctx));

    Pin* outPin = dmxOut->GetOutputPin("output");
    ASSERT_NE(outPin, nullptr);
    ChannelBuffer outBuf = outPin->GetValue().Get<ChannelBuffer>();
    EXPECT_EQ(outBuf.GetChannelCount(), 4u);
}
