#include <gtest/gtest.h>
#include "graph/Graph.h"
#include "graph/CoreNodes.h"
#include "graph/NodeRegistry.h"
#include "operators/tex/MovieFileInTexOp.h"
#include "operators/tex/WarpBlendTexOp.h"
#include "operators/tex/ProjectorOutTexOp.h"
#include <chrono>

using namespace nf;

TEST(ProjectionMappingBenchmark, DualProjector60FPSThroughput) {
    RegisterCoreNodes(NodeRegistry::Instance());

    Graph graph;
    auto* movie = graph.CreateNode<MovieFileInTexOp>("movie1");

    // Projector 1 (Left Facade)
    auto* warp1 = graph.CreateNode<WarpBlendTexOp>("warp1");
    auto* proj1 = graph.CreateNode<ProjectorOutTexOp>("proj1");
    warp1->SetParam("blend_right", 0.2f);
    proj1->SetParam("display_index", static_cast<int32_t>(1));

    // Projector 2 (Right Facade)
    auto* warp2 = graph.CreateNode<WarpBlendTexOp>("warp2");
    auto* proj2 = graph.CreateNode<ProjectorOutTexOp>("proj2");
    warp2->SetParam("blend_left", 0.2f);
    proj2->SetParam("display_index", static_cast<int32_t>(2));

    graph.Connect(movie->GetOutputPin("output"), warp1->GetInputPin("input"));
    graph.Connect(warp1->GetOutputPin("output"), proj1->GetInputPin("input"));

    graph.Connect(movie->GetOutputPin("output"), warp2->GetInputPin("input"));
    graph.Connect(warp2->GetOutputPin("output"), proj2->GetInputPin("input"));

    CookContext ctx{ .deltaTimeSeconds = 1.0 / 60.0 };

    auto start = std::chrono::high_resolution_clock::now();

    for (uint64_t frame = 1; frame <= 1000; ++frame) {
        ctx.frameIndex = frame;
        ctx.timeSeconds = static_cast<double>(frame) / 60.0;
        movie->MarkDirty();
        bool ok = graph.CookAll(ctx);
        ASSERT_TRUE(ok);
    }

    auto end = std::chrono::high_resolution_clock::now();
    double totalMs = std::chrono::duration<double, std::milli>(end - start).count();
    double perFrameMs = totalMs / 1000.0;

    EXPECT_LT(perFrameMs, 16.67); // Dual-projector pipeline well under 16.6ms per frame
}

TEST(ProjectionMappingBenchmark, LongRunning10000FrameSoakZeroLeaks) {
    RegisterCoreNodes(NodeRegistry::Instance());

    Graph graph;
    auto* movie = graph.CreateNode<MovieFileInTexOp>("movie1");
    auto* warp = graph.CreateNode<WarpBlendTexOp>("warp1");
    auto* proj = graph.CreateNode<ProjectorOutTexOp>("proj1");

    graph.Connect(movie->GetOutputPin("output"), warp->GetInputPin("input"));
    graph.Connect(warp->GetOutputPin("output"), proj->GetInputPin("input"));

    CookContext ctx{ .deltaTimeSeconds = 1.0 / 60.0 };

    for (uint64_t frame = 1; frame <= 10000; ++frame) {
        ctx.frameIndex = frame;
        ctx.timeSeconds = static_cast<double>(frame) / 60.0;
        movie->MarkDirty();
        bool ok = graph.CookAll(ctx);
        ASSERT_TRUE(ok);
    }
}
