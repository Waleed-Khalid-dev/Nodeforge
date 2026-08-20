#include <gtest/gtest.h>
#include "diagnostics/CrashReporter.h"
#include "graph/Graph.h"
#include "graph/CoreNodes.h"
#include "graph/NodeRegistry.h"
#include "graph/GraphSerializer.h"
#include "operators/chan/ConstantChanOp.h"
#include "operators/chan/MathChanOp.h"
#include <filesystem>
#include <fstream>

using namespace nf;

TEST(CrashRecoveryTest, EmergencySnapshotSerializationAndRestore) {
    RegisterCoreNodes(NodeRegistry::Instance());

    Graph graph;
    auto* constOp = graph.CreateNode<ConstantChanOp>("src_const");
    auto* mathOp = graph.CreateNode<MathChanOp>("math_proc");
    graph.Connect(constOp->GetOutputPin("output"), mathOp->GetInputPin("input1"));

    const std::string testCrashPath = (std::filesystem::temp_directory_path() / "test_emergency_recovery.nfp").string();
    if (std::filesystem::exists(testCrashPath)) {
        std::filesystem::remove(testCrashPath);
    }

    auto& crashReporter = CrashReporter::Instance();
    crashReporter.SetActiveGraph(&graph);
    crashReporter.SetCrashDumpPath(testCrashPath);

    bool hookCalled = false;
    crashReporter.SetCustomCrashHook([&](const std::string& reason) {
        hookCalled = true;
        EXPECT_EQ(reason, "Simulated Test Fault");
    });

    EXPECT_TRUE(crashReporter.WriteEmergencyCrashSnapshot("Simulated Test Fault"));
    EXPECT_TRUE(hookCalled);
    EXPECT_TRUE(std::filesystem::exists(testCrashPath));

    // Verify snapshot contents
    std::ifstream file(testCrashPath);
    ASSERT_TRUE(file.is_open());
    nlohmann::json root = nlohmann::json::parse(file);
    file.close();

    EXPECT_EQ(root["format_version"], 1);
    EXPECT_EQ(root["generator"], "NodeForge CrashReporter");
    EXPECT_EQ(root["crash_reason"], "Simulated Test Fault");
    EXPECT_TRUE(root.contains("graph"));

    // Restore graph from crash snapshot
    Graph restoredGraph;
    std::string err;
    EXPECT_TRUE(GraphSerializer::Deserialize(restoredGraph, root["graph"], NodeRegistry::Instance(), &err));
    EXPECT_EQ(restoredGraph.GetNodes().size(), 2u);
    EXPECT_NE(restoredGraph.FindNode("src_const"), nullptr);
    EXPECT_NE(restoredGraph.FindNode("math_proc"), nullptr);

    // Clean up test file
    std::filesystem::remove(testCrashPath);
}
