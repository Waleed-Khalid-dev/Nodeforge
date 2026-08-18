#include <gtest/gtest.h>
#include "graph/Graph.h"
#include "operators/chan/ConstantChanOp.h"
#include "operators/chan/MathChanOp.h"
#include <chrono>
#include <spdlog/spdlog.h>

TEST(GraphBenchmarkTest, DirtyPropagation1000NodesUnder1ms) {
    nf::Graph graph;
    constexpr int NODE_COUNT = 1000;

    auto* root = graph.CreateNode<nf::ConstantChanOp>("Root");
    nf::Node* prev = root;

    for (int i = 1; i < NODE_COUNT; ++i) {
        auto* next = graph.CreateNode<nf::MathChanOp>("Node_" + std::to_string(i));
        graph.Connect(prev->GetOutputPin("output"), next->GetInputPin("input"));
        prev = next;
    }

    // Cook all nodes first to clear dirty flags
    nf::CookContext ctx{ .frameIndex = 1 };
    graph.CookAll(ctx);

    for (const auto& [id, node] : graph.GetNodes()) {
        ASSERT_FALSE(node->IsDirty());
    }

    // Measure dirty propagation latency
    constexpr int ITERATIONS = 1000;
    double totalDurationUs = 0.0;

    for (int it = 0; it < ITERATIONS; ++it) {
        // Clear dirty flags outside the timed region
        for (const auto& [id, node] : graph.GetNodes()) {
            node->ClearDirty();
        }

        // Measure ONLY the dirty propagation pass
        auto start = std::chrono::high_resolution_clock::now();
        root->MarkDirty();
        auto end = std::chrono::high_resolution_clock::now();

        totalDurationUs += std::chrono::duration<double, std::micro>(end - start).count();
    }

    double avgUs = totalDurationUs / ITERATIONS;
    double avgMs = avgUs / 1000.0;

    spdlog::info("Dirty propagation across {} nodes: {:.4f} ms ({:.2f} microseconds)", NODE_COUNT, avgMs, avgUs);

    // Roadmap DoD target: < 1.0 ms
    EXPECT_LT(avgMs, 1.0);
}
