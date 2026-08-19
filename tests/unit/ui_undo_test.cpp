#include <gtest/gtest.h>
#include "../../src/ui/EditorContext.h"
#include "../../src/ui/commands/NodeCommands.h"
#include "../../src/ui/commands/WireCommands.h"
#include "../../src/ui/commands/ParamCommands.h"
#include "../../src/graph/CoreNodes.h"

using namespace nf;
using namespace nf::ui;

class UIUndoTest : public ::testing::Test {
protected:
    void SetUp() override {
        RegisterCoreNodes(NodeRegistry::Instance());
        graph = std::make_unique<Graph>();
        ctx = std::make_unique<EditorContext>();
        ctx->SetGraph(graph.get());
    }

    std::unique_ptr<Graph> graph;
    std::unique_ptr<EditorContext> ctx;
};

TEST_F(UIUndoTest, NodeCreateAndUndoRedo) {
    auto cmd = std::make_unique<CreateNodeCommand>(ctx.get(), "ConstantChanOp", "Const1", glm::vec2(100.0f, 200.0f));
    NodeId id = cmd->GetCreatedNodeId();
    ctx->GetUndoManager().ExecuteCommand(std::move(cmd));

    EXPECT_EQ(graph->GetNodes().size(), 1u);
    EXPECT_TRUE(ctx->IsNodeSelected(id != 0 ? id : 1));

    // Undo
    EXPECT_TRUE(ctx->GetUndoManager().Undo());
    EXPECT_EQ(graph->GetNodes().size(), 0u);

    // Redo
    EXPECT_TRUE(ctx->GetUndoManager().Redo());
    EXPECT_EQ(graph->GetNodes().size(), 1u);
}

TEST_F(UIUndoTest, WireConnectAndUndoRedo) {
    auto n1 = NodeRegistry::Instance().CreateNode("ConstantChanOp", graph->GenerateNodeId(), "Const1");
    auto n2 = NodeRegistry::Instance().CreateNode("MathChanOp", graph->GenerateNodeId(), "Math1");
    NodeId id1 = n1->GetId();
    NodeId id2 = n2->GetId();

    graph->AddNode(std::move(n1));
    graph->AddNode(std::move(n2));

    Pin* p1 = graph->GetNode(id1)->GetOutputPin(0);
    Pin* p2 = graph->GetNode(id2)->GetInputPin(0);

    auto cmd = std::make_unique<ConnectWireCommand>(ctx.get(), p1, p2);
    ctx->GetUndoManager().ExecuteCommand(std::move(cmd));

    EXPECT_EQ(graph->GetWires().size(), 1u);
    EXPECT_TRUE(p2->IsConnected());

    // Undo
    EXPECT_TRUE(ctx->GetUndoManager().Undo());
    EXPECT_EQ(graph->GetWires().size(), 0u);
    EXPECT_FALSE(p2->IsConnected());

    // Redo
    EXPECT_TRUE(ctx->GetUndoManager().Redo());
    EXPECT_EQ(graph->GetWires().size(), 1u);
    EXPECT_TRUE(p2->IsConnected());
}

TEST_F(UIUndoTest, ParameterValueChangeAndUndo) {
    auto n = NodeRegistry::Instance().CreateNode("ConstantChanOp", graph->GenerateNodeId(), "Const1");
    NodeId id = n->GetId();
    graph->AddNode(std::move(n));

    Parameter* p = graph->GetNode(id)->GetParams().Get("sample_rate");
    ASSERT_NE(p, nullptr);
    p->SetValue(PinValue(60.0f));

    auto cmd = std::make_unique<SetParamValueCommand>(ctx.get(), id, "sample_rate", PinValue(60.0f), PinValue(120.0f));
    ctx->GetUndoManager().ExecuteCommand(std::move(cmd));

    EXPECT_FLOAT_EQ(p->GetValue().Get<float>(), 120.0f);

    // Undo
    EXPECT_TRUE(ctx->GetUndoManager().Undo());
    EXPECT_FLOAT_EQ(p->GetValue().Get<float>(), 60.0f);

    // Redo
    EXPECT_TRUE(ctx->GetUndoManager().Redo());
    EXPECT_FLOAT_EQ(p->GetValue().Get<float>(), 120.0f);
}

TEST_F(UIUndoTest, NodeMoveCoalescing) {
    std::unordered_map<NodeId, glm::vec2> pos0 = { { 1, glm::vec2(0.0f, 0.0f) } };
    std::unordered_map<NodeId, glm::vec2> pos1 = { { 1, glm::vec2(10.0f, 10.0f) } };
    std::unordered_map<NodeId, glm::vec2> pos2 = { { 1, glm::vec2(20.0f, 20.0f) } };

    ctx->GetUndoManager().ExecuteCommand(std::make_unique<MoveNodesCommand>(ctx.get(), pos0, pos1));
    ctx->GetUndoManager().ExecuteCommand(std::make_unique<MoveNodesCommand>(ctx.get(), pos1, pos2));

    // Coalesced into 1 command
    EXPECT_EQ(ctx->GetUndoManager().GetUndoCount(), 1u);
    EXPECT_EQ(ctx->GetNodePosition(1), glm::vec2(20.0f, 20.0f));

    EXPECT_TRUE(ctx->GetUndoManager().Undo());
    EXPECT_EQ(ctx->GetNodePosition(1), glm::vec2(0.0f, 0.0f));
}
