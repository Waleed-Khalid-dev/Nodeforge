#include <gtest/gtest.h>
#include "graph/NodeRegistry.h"
#include "graph/CoreNodes.h"
#include "operators/chan/ConstantChanOp.h"
#include "operators/chan/MathChanOp.h"
#include "operators/tex/ConstantTexOp.h"
#include "operators/tex/TransformTexOp.h"

TEST(OperatorsTest, ConstantChanOpOutputs) {
    nf::ConstantChanOp op(1, "Const1");
    op.SetParam("channel_names", std::string("tx ty tz"));
    op.SetParam("values", glm::vec4(1.5f, 2.5f, 3.5f, 0.0f));
    op.SetParam("sample_count", static_cast<int32_t>(10));

    nf::CookContext ctx;
    EXPECT_TRUE(op.Cook(ctx));

    const auto& outVal = op.GetOutputPin("output")->GetValue();
    ASSERT_TRUE(outVal.Is<nf::ChannelBuffer>());
    const auto& buf = outVal.Get<nf::ChannelBuffer>();

    ASSERT_EQ(buf.GetChannelCount(), 3u);
    EXPECT_EQ(buf.GetChannelNames()[0], "tx");
    EXPECT_EQ(buf.GetChannelNames()[1], "ty");
    EXPECT_EQ(buf.GetChannelNames()[2], "tz");

    ASSERT_EQ(buf.GetSampleCount(), 10u);
    EXPECT_FLOAT_EQ(buf.GetSample(0, 0), 1.5f);
    EXPECT_FLOAT_EQ(buf.GetSample(1, 0), 2.5f);
    EXPECT_FLOAT_EQ(buf.GetSample(2, 0), 3.5f);
}

TEST(OperatorsTest, MathChanOpMathCalculations) {
    nf::ConstantChanOp constOp(1, "Const");
    constOp.SetParam("channel_names", std::string("chan1"));
    constOp.SetParam("values", glm::vec4(3.0f, 0.0f, 0.0f, 0.0f));

    nf::MathChanOp mathOp(2, "Math");
    mathOp.GetInputPin("input1")->AddConnection(constOp.GetOutputPin("output"));

    nf::CookContext ctx;
    EXPECT_TRUE(constOp.Cook(ctx));

    // Test 1: Multiply by 4.0 -> 12.0
    mathOp.SetParam("operation", static_cast<int32_t>(1)); // Multiply
    mathOp.SetParam("scalar", 4.0f);
    EXPECT_TRUE(mathOp.Cook(ctx));
    EXPECT_FLOAT_EQ(mathOp.GetOutputPin("output")->GetValue().Get<nf::ChannelBuffer>().GetSample(0, 0), 12.0f);

    // Test 2: Range remap from [0, 10] to [0, 100] on value 3.0 -> 30.0
    mathOp.SetParam("operation", static_cast<int32_t>(0)); // Add
    mathOp.SetParam("scalar", 0.0f);
    mathOp.SetParam("range_in_min", 0.0f);
    mathOp.SetParam("range_in_max", 10.0f);
    mathOp.SetParam("range_out_min", 0.0f);
    mathOp.SetParam("range_out_max", 100.0f);
    EXPECT_TRUE(mathOp.Cook(ctx));
    EXPECT_FLOAT_EQ(mathOp.GetOutputPin("output")->GetValue().Get<nf::ChannelBuffer>().GetSample(0, 0), 30.0f);
}

TEST(OperatorsTest, NodeRegistryCoreRegistration) {
    nf::NodeRegistry registry;
    nf::RegisterCoreNodes(registry);

    auto allTypes = registry.GetAllTypes();
    EXPECT_GE(allTypes.size(), 4u);
    EXPECT_NE(registry.GetTypeInfo("ConstantChanOp"), nullptr);
    EXPECT_NE(registry.GetTypeInfo("MathChanOp"), nullptr);
    EXPECT_NE(registry.GetTypeInfo("ConstantTexOp"), nullptr);
    EXPECT_NE(registry.GetTypeInfo("TransformTexOp"), nullptr);

    auto node = registry.CreateNode("ConstantChanOp", 10, "InstancedConst");
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->GetName(), "InstancedConst");
    EXPECT_EQ(node->GetTypeName(), "ConstantChanOp");
}
