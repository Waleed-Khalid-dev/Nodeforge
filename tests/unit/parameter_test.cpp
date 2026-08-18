#include <gtest/gtest.h>
#include "param/ParameterGroup.h"
#include "graph/Graph.h"
#include "operators/chan/ConstantChanOp.h"
#include "python/PythonEngine.h"

TEST(ParameterTest, BasicTypesAndLimits) {
    nf::ParameterGroup group;

    auto* floatParam = group.AddFloat("speed", "Speed", 1.0f, 0.0f, 10.0f, true);
    ASSERT_NE(floatParam, nullptr);
    EXPECT_EQ(floatParam->GetName(), "speed");
    EXPECT_FLOAT_EQ(floatParam->AsFloat(), 1.0f);

    // Value within limits
    floatParam->SetValue(nf::PinValue(5.0f));
    EXPECT_FLOAT_EQ(floatParam->AsFloat(), 5.0f);

    // Value exceeding upper limit -> clamped to 10.0
    floatParam->SetValue(nf::PinValue(25.0f));
    EXPECT_FLOAT_EQ(floatParam->AsFloat(), 10.0f);

    // Value below lower limit -> clamped to 0.0
    floatParam->SetValue(nf::PinValue(-5.0f));
    EXPECT_FLOAT_EQ(floatParam->AsFloat(), 0.0f);

    // Integer param limits
    auto* intParam = group.AddInt("count", "Count", 4, 1, 8, true);
    intParam->SetValue(nf::PinValue(100));
    EXPECT_EQ(intParam->AsInt(), 8);
}

TEST(ParameterTest, ParameterGroupPagesAndRetrieval) {
    nf::ParameterGroup group;

    group.AddFloat("w", "Width", 1920.0f, 0.0f, 4096.0f, false, "Dimensions");
    group.AddFloat("h", "Height", 1080.0f, 0.0f, 4096.0f, false, "Dimensions");
    group.AddBool("active", "Active", true, "Settings");

    auto pages = group.GetPages();
    EXPECT_EQ(pages.size(), 2u);

    auto dimParams = group.GetParametersByPage("Dimensions");
    EXPECT_EQ(dimParams.size(), 2u);

    auto setParams = group.GetParametersByPage("Settings");
    EXPECT_EQ(setParams.size(), 1u);
}

TEST(ParameterTest, ExpressionEvaluationViaCook) {
    nf::PythonEngine::Instance().Initialize();

    nf::Graph graph;
    nf::PythonEngine::Instance().SetActiveGraph(&graph);

    auto* op = graph.CreateNode<nf::ConstantChanOp>("Generator");
    auto* param = op->GetParams().Get("values");
    ASSERT_NE(param, nullptr);

    // Set expression on parameter
    param->SetExpression("10.0 * 2.5");
    EXPECT_EQ(param->GetMode(), nf::ParamMode::Expression);

    nf::CookContext ctx{ .frameIndex = 1, .timeSeconds = 0.0 };
    EXPECT_TRUE(op->EnsureCooked(ctx));

    // Evaluated value: 10.0 * 2.5 = 25.0
    EXPECT_FLOAT_EQ(param->AsFloat(), 25.0f);

    // Time-based expression
    param->SetExpression("math.sin(time) + 5.0");
    ctx.timeSeconds = 0.0;
    ctx.frameIndex = 2;
    op->MarkDirty();
    EXPECT_TRUE(op->EnsureCooked(ctx));
    EXPECT_FLOAT_EQ(param->AsFloat(), 5.0f); // sin(0) + 5 = 5.0
}
