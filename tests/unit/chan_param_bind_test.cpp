#include <gtest/gtest.h>
#include "graph/Graph.h"
#include "graph/CoreNodes.h"
#include "graph/NodeRegistry.h"
#include "operators/chan/LFOChanOp.h"
#include "operators/tex/NoiseTexOp.h"
#include "python/PythonEngine.h"
#include <cmath>

using namespace nf;

class ChanParamBindTest : public ::testing::Test {
protected:
    void SetUp() override {
        RegisterCoreNodes(NodeRegistry::Instance());
        PythonEngine::Instance().Initialize();
    }
};

TEST_F(ChanParamBindTest, NativeParameterChannelBinding) {
    Graph graph;
    PythonEngine::Instance().SetActiveGraph(&graph);

    auto* lfo = graph.CreateNode<LFOChanOp>("lfo1");
    auto* noise = graph.CreateNode<NoiseTexOp>("noise1");

    lfo->SetParam("type", static_cast<int32_t>(0)); // Sine
    lfo->SetParam("frequency", 2.0f);
    lfo->SetParam("amplitude", 5.0f);
    lfo->SetParam("offset", 10.0f);
    lfo->SetParam("channel_names", std::string("freq"));

    // Directly bind NoiseTexOp's period parameter to lfo1's freq channel
    Parameter* periodParam = noise->GetParams().Get("period");
    ASSERT_NE(periodParam, nullptr);
    periodParam->SetBoundChannel("lfo1", "freq");
    EXPECT_EQ(periodParam->GetMode(), ParamMode::BoundChannel);

    // Cook frame 1 at t = 0s
    CookContext ctx0{ .frameIndex = 1, .timeSeconds = 0.0, .deltaTimeSeconds = 1.0 / 60.0 };
    EXPECT_TRUE(graph.CookAll(ctx0));

    // offset + amp * sin(0) = 10.0
    EXPECT_NEAR(periodParam->AsFloat(), 10.0f, 1e-4f);

    // Cook frame 15 at t = 0.125s (90 deg for 2Hz)
    CookContext ctx90{ .frameIndex = 15, .timeSeconds = 0.125, .deltaTimeSeconds = 1.0 / 60.0 };
    EXPECT_TRUE(graph.CookAll(ctx90));

    // offset + amp * sin(pi/2) = 10.0 + 5.0 = 15.0
    EXPECT_NEAR(periodParam->AsFloat(), 15.0f, 1e-4f);
}

TEST_F(ChanParamBindTest, PythonExpressionChannelSubscript) {
    Graph graph;
    PythonEngine::Instance().SetActiveGraph(&graph);

    auto* lfo = graph.CreateNode<LFOChanOp>("lfo1");
    auto* noise = graph.CreateNode<NoiseTexOp>("noise1");

    lfo->SetParam("type", static_cast<int32_t>(0));
    lfo->SetParam("frequency", 1.0f);
    lfo->SetParam("amplitude", 3.0f);
    lfo->SetParam("offset", 5.0f);
    lfo->SetParam("channel_names", std::string("amp"));

    // Bind expression op('lfo1')['amp']
    Parameter* roughParam = noise->GetParams().Get("roughness");
    ASSERT_NE(roughParam, nullptr);
    roughParam->SetExpression("op('lfo1')['amp']");

    CookContext ctx0{ .frameIndex = 1, .timeSeconds = 0.0, .deltaTimeSeconds = 1.0 / 60.0 };
    EXPECT_TRUE(graph.CookAll(ctx0));
    EXPECT_NEAR(roughParam->AsFloat(), 5.0f, 1e-3f);
}
