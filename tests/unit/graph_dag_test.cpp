#include <gtest/gtest.h>
#include "graph/Graph.h"
#include "graph/GraphSerializer.h"
#include "graph/NodeRegistry.h"
#include "graph/CoreNodes.h"
#include "operators/chan/ConstantChanOp.h"
#include "operators/chan/MathChanOp.h"

namespace {

class CookCounterNode : public nf::Node {
public:
    CookCounterNode(nf::NodeId id, const std::string& name)
        : nf::Node(id, name, "CookCounterNode") {
        AddInputPin("input", nf::PinType::Chan);
        m_outPin = AddOutputPin("output", nf::PinType::Chan);
    }

    bool Cook(const nf::CookContext& /*context*/) override {
        cookCount++;
        nf::ChannelBuffer buf;
        buf.AddChannel("counter", { static_cast<float>(cookCount) });
        m_outPin->SetValue(nf::PinValue(buf));
        return true;
    }

    int cookCount = 0;
    nf::Pin* m_outPin = nullptr;
};

} // namespace

TEST(GraphDAGTest, LinearChain100Nodes) {
    nf::Graph graph;
    nf::CookContext ctx{ .frameIndex = 1, .timeSeconds = 0.0, .deltaTimeSeconds = 1.0 / 60.0 };

    // Create root generator
    auto* root = graph.CreateNode<nf::ConstantChanOp>("Root");
    root->SetParam("channel_names", std::string("val"));
    root->SetParam("values", glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));

    nf::Node* prevNode = root;
    for (int i = 1; i < 100; ++i) {
        auto* nextNode = graph.CreateNode<nf::MathChanOp>("Node_" + std::to_string(i));
        nextNode->SetParam("operation", static_cast<int32_t>(0)); // Add
        nextNode->SetParam("scalar", 1.0f); // Add 1 at each step

        auto* wire = graph.Connect(prevNode->GetOutputPin("output"), nextNode->GetInputPin("input"));
        ASSERT_NE(wire, nullptr) << "Failed connecting node " << (i - 1) << " to " << i;
        prevNode = nextNode;
    }

    // Cook the last node
    bool ok = graph.CookNode(prevNode, ctx);
    EXPECT_TRUE(ok);

    const nf::PinValue& outVal = prevNode->GetOutputPin("output")->GetValue();
    ASSERT_TRUE(outVal.Is<nf::ChannelBuffer>());
    const auto& buf = outVal.Get<nf::ChannelBuffer>();
    ASSERT_EQ(buf.GetChannelCount(), 1u);
    ASSERT_EQ(buf.GetSampleCount(), 1u);
    // Root is 1.0 + 99 additions of 1.0 = 100.0
    EXPECT_FLOAT_EQ(buf.GetSample(0, 0), 100.0f);
}

TEST(GraphDAGTest, DiamondGraphCooksOncePerFrame) {
    nf::Graph graph;
    nf::CookContext ctx{ .frameIndex = 1, .timeSeconds = 0.0, .deltaTimeSeconds = 1.0 / 60.0 };

    // Diamond: A -> B, A -> C, B -> D, C -> D (or D with 2 inputs)
    auto* nodeA = graph.CreateNode<CookCounterNode>("NodeA");
    auto* nodeB = graph.CreateNode<nf::MathChanOp>("NodeB");
    auto* nodeC = graph.CreateNode<nf::MathChanOp>("NodeC");
    auto* nodeD = graph.CreateNode<nf::MathChanOp>("NodeD");

    nodeB->SetParam("scalar", 10.0f);
    nodeC->SetParam("scalar", 20.0f);

    ASSERT_NE(graph.Connect(nodeA->GetOutputPin("output"), nodeB->GetInputPin("input")), nullptr);
    ASSERT_NE(graph.Connect(nodeA->GetOutputPin("output"), nodeC->GetInputPin("input")), nullptr);
    ASSERT_NE(graph.Connect(nodeB->GetOutputPin("output"), nodeD->GetInputPin("input")), nullptr);

    // Cook graph for frame 1
    bool ok = graph.CookAll(ctx);
    EXPECT_TRUE(ok);

    // Node A must cook EXACTLY 1 time, despite having 2 downstream branches (B & C)
    EXPECT_EQ(nodeA->cookCount, 1);

    // Cooking frame 1 again without dirtying must not increment cook count (memoization)
    graph.CookAll(ctx);
    EXPECT_EQ(nodeA->cookCount, 1);

    // Advancing frame and marking dirty
    ctx.frameIndex = 2;
    nodeA->MarkDirty();
    graph.CookAll(ctx);
    EXPECT_EQ(nodeA->cookCount, 2);
}

TEST(GraphDAGTest, CycleRejection) {
    nf::Graph graph;

    auto* nodeA = graph.CreateNode<nf::MathChanOp>("NodeA");
    auto* nodeB = graph.CreateNode<nf::MathChanOp>("NodeB");
    auto* nodeC = graph.CreateNode<nf::MathChanOp>("NodeC");

    ASSERT_NE(graph.Connect(nodeA->GetOutputPin("output"), nodeB->GetInputPin("input")), nullptr);
    ASSERT_NE(graph.Connect(nodeB->GetOutputPin("output"), nodeC->GetInputPin("input")), nullptr);

    // Self-loop connection (A -> A) must be rejected
    std::string err;
    auto* wireSelf = graph.Connect(nodeA->GetOutputPin("output"), nodeA->GetInputPin("input"), &err);
    EXPECT_EQ(wireSelf, nullptr);
    EXPECT_FALSE(err.empty());

    // Cycle connection (C -> A) must be rejected
    auto* wireCycle = graph.Connect(nodeC->GetOutputPin("output"), nodeA->GetInputPin("input"), &err);
    EXPECT_EQ(wireCycle, nullptr);
    EXPECT_TRUE(graph.WouldCreateCycle(nodeC->GetOutputPin("output"), nodeA->GetInputPin("input")));
}

TEST(GraphDAGTest, DirtyPropagation) {
    nf::Graph graph;
    nf::CookContext ctx{ .frameIndex = 1 };

    auto* nodeA = graph.CreateNode<nf::ConstantChanOp>("NodeA");
    auto* nodeB = graph.CreateNode<nf::MathChanOp>("NodeB");
    auto* nodeC = graph.CreateNode<nf::MathChanOp>("NodeC");

    graph.Connect(nodeA->GetOutputPin("output"), nodeB->GetInputPin("input"));
    graph.Connect(nodeB->GetOutputPin("output"), nodeC->GetInputPin("input"));

    // Initial state: all dirty
    EXPECT_TRUE(nodeA->IsDirty());
    EXPECT_TRUE(nodeB->IsDirty());
    EXPECT_TRUE(nodeC->IsDirty());

    // Cook all
    graph.CookAll(ctx);
    EXPECT_FALSE(nodeA->IsDirty());
    EXPECT_FALSE(nodeB->IsDirty());
    EXPECT_FALSE(nodeC->IsDirty());

    // Modify parameter on A
    nodeA->SetParam("values", glm::vec4(5.0f, 0.0f, 0.0f, 0.0f));

    // A, B, and C must all be dirty now due to downstream propagation
    EXPECT_TRUE(nodeA->IsDirty());
    EXPECT_TRUE(nodeB->IsDirty());
    EXPECT_TRUE(nodeC->IsDirty());
}

TEST(GraphSerializerTest, JsonRoundtripSimpleGraph) {
    nf::NodeRegistry registry;
    nf::RegisterCoreNodes(registry);

    nf::Graph graph;
    auto* nodeA = graph.CreateNode<nf::ConstantChanOp>("Generator");
    nodeA->SetParam("channel_names", std::string("test_chan"));
    nodeA->SetParam("values", glm::vec4(42.0f, 0.0f, 0.0f, 0.0f));

    auto* nodeB = graph.CreateNode<nf::MathChanOp>("Multiplier");
    nodeB->SetParam("operation", static_cast<int32_t>(1)); // Multiply
    nodeB->SetParam("scalar", 2.0f);

    ASSERT_NE(graph.Connect(nodeA->GetOutputPin("output"), nodeB->GetInputPin("input")), nullptr);

    // Serialize to JSON
    nlohmann::json json = nf::GraphSerializer::Serialize(graph);
    EXPECT_EQ(json["nodes"].size(), 2u);
    EXPECT_EQ(json["wires"].size(), 1u);

    // Deserialize into new graph
    nf::Graph restoredGraph;
    std::string err;
    bool ok = nf::GraphSerializer::Deserialize(restoredGraph, json, registry, &err);
    ASSERT_TRUE(ok) << "Deserialization error: " << err;

    EXPECT_EQ(restoredGraph.GetNodes().size(), 2u);
    EXPECT_EQ(restoredGraph.GetWires().size(), 1u);

    // Cook restored graph
    nf::CookContext ctx{ .frameIndex = 1 };
    EXPECT_TRUE(restoredGraph.CookAll(ctx));

    auto* restoredB = restoredGraph.FindNode("Multiplier");
    ASSERT_NE(restoredB, nullptr);
    const auto& outVal = restoredB->GetOutputPin("output")->GetValue();
    // 42.0 * 2.0 = 84.0
    EXPECT_FLOAT_EQ(outVal.Get<nf::ChannelBuffer>().GetSample(0, 0), 84.0f);
}
