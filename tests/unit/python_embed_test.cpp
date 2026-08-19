#include <gtest/gtest.h>
#include "python/PythonEngine.h"
#include "graph/Graph.h"
#include "graph/NodeRegistry.h"
#include "graph/CoreNodes.h"
#include "operators/chan/ConstantChanOp.h"
#include "operators/chan/MathChanOp.h"

TEST(PythonEmbedTest, CreateNodesAndConnectViaPython) {
    nf::NodeRegistry::Instance().Clear();
    nf::RegisterCoreNodes(nf::NodeRegistry::Instance());

    nf::PythonEngine::Instance().Initialize();
    ASSERT_TRUE(nf::PythonEngine::Instance().IsInitialized());

    nf::Graph graph;
    nf::PythonEngine::Instance().SetActiveGraph(&graph);

    const std::string script = R"(
import nodeforge

# Create operators via Python script
gen = nodeforge.create_node("ConstantChanOp", "PyGen")
math = nodeforge.create_node("MathChanOp", "PyMath")

# Connect them
nodeforge.connect(gen, "output", math, "input")

# Set parameter values
gen.set_param("values", 15.0)
math.set_param("operation", 1) # Multiply
math.set_param("scalar", 3.0)
)";

    std::string err;
    bool ok = nf::PythonEngine::Instance().ExecuteString(script, &err);
    ASSERT_TRUE(ok) << "Python error: " << err;

    // Verify graph structure in C++
    EXPECT_EQ(graph.GetNodes().size(), 2u);
    EXPECT_EQ(graph.GetWires().size(), 1u);

    auto* pyMath = graph.FindNode("PyMath");
    ASSERT_NE(pyMath, nullptr);

    // Cook and evaluate
    nf::CookContext ctx{ .frameIndex = 1 };
    EXPECT_TRUE(graph.CookAll(ctx));

    const auto& outVal = pyMath->GetOutputPin("output")->GetValue();
    ASSERT_TRUE(outVal.Is<nf::ChannelBuffer>());
    // 15.0 * 3.0 = 45.0
    EXPECT_FLOAT_EQ(outVal.Get<nf::ChannelBuffer>().GetSample(0, 0), 45.0f);
}

TEST(PythonEmbedTest, ParameterExpressionsReferencingOtherNodes) {
    nf::NodeRegistry::Instance().Clear();
    nf::RegisterCoreNodes(nf::NodeRegistry::Instance());

    nf::PythonEngine::Instance().Initialize();

    nf::Graph graph;
    nf::PythonEngine::Instance().SetActiveGraph(&graph);

    auto* nodeA = graph.CreateNode<nf::ConstantChanOp>("SourceNode");
    nodeA->SetParam("values", glm::vec4(21.0f, 0.0f, 0.0f, 0.0f));

    auto* nodeB = graph.CreateNode<nf::ConstantChanOp>("TargetNode");
    auto* param = nodeB->GetParams().Get("values");
    ASSERT_NE(param, nullptr);

    // Dynamic expression referencing nodeA
    param->SetExpression("op('SourceNode').get_param('values').as_float() * 2.0");

    nf::CookContext ctx{ .frameIndex = 1 };
    EXPECT_TRUE(graph.CookAll(ctx));

    // 21.0 * 2.0 = 42.0
    EXPECT_NEAR(static_cast<double>(param->AsFloat()), 42.0, 1e-4);
}

TEST(PythonEmbedTest, ExceptionSafetyAndRecovery) {
    nf::PythonEngine::Instance().Initialize();

    std::string err;
    // Division by zero in script
    bool ok = nf::PythonEngine::Instance().ExecuteString("x = 1.0 / 0.0", &err);
    EXPECT_FALSE(ok);
    EXPECT_FALSE(err.empty());

    // Host process survives and can execute subsequent valid Python code
    ok = nf::PythonEngine::Instance().ExecuteString("y = 42 + 8", &err);
    EXPECT_TRUE(ok);
}
