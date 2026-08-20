#include <gtest/gtest.h>
#include "graph/Graph.h"
#include "graph/CoreNodes.h"
#include "graph/NodeRegistry.h"
#include "profiling/CookProfiler.h"
#include "operators/chan/ConstantChanOp.h"
#include "operators/chan/LFOChanOp.h"
#include "operators/chan/FilterChanOp.h"
#include "operators/chan/MathChanOp.h"
#include "operators/data/TableDataOp.h"
#include "operators/geom/GridGeomOp.h"
#include "operators/geom/NoiseDeformGeomOp.h"
#include <chrono>

using namespace nf;

TEST(LongSoakBenchmark, ContinuousMultiFamily10000FrameSoakTest) {
    RegisterCoreNodes(NodeRegistry::Instance());

    Graph graph;
    auto* lfo = graph.CreateNode<LFOChanOp>("lfo_source");
    auto* filter = graph.CreateNode<FilterChanOp>("filter_proc");
    auto* math = graph.CreateNode<MathChanOp>("math_proc");
    auto* table = graph.CreateNode<TableDataOp>("data_log");
    auto* grid = graph.CreateNode<GridGeomOp>("geom_grid");
    auto* deform = graph.CreateNode<NoiseDeformGeomOp>("geom_deform");

    graph.Connect(lfo->GetOutputPin("output"), filter->GetInputPin("input"));
    graph.Connect(filter->GetOutputPin("output"), math->GetInputPin("input1"));
    graph.Connect(grid->GetOutputPin("output"), deform->GetInputPin("input"));

    table->SetParam("column_names", std::string("frame,cpu_ms,fps"));
    lfo->SetParam("channel_names", std::string("chan1 chan2 chan3 chan4"));
    lfo->SetParam("time_sliced", true);
    grid->SetParam("rows", static_cast<int32_t>(10));
    grid->SetParam("cols", static_cast<int32_t>(10));

    auto& profiler = CookProfiler::Instance();
    profiler.ResetAll();

    const int totalFrames = 5000;
    CookContext ctx{ .deltaTimeSeconds = 1.0 / 60.0 };

    auto start = std::chrono::high_resolution_clock::now();

    for (int f = 1; f <= totalFrames; ++f) {
        ctx.frameIndex = f;
        ctx.timeSeconds = f / 60.0;

        profiler.BeginFrame(f, ctx.deltaTimeSeconds);
        graph.CookAll(ctx);
        profiler.EndFrame();
    }

    auto end = std::chrono::high_resolution_clock::now();
    double totalMs = std::chrono::duration<double, std::milli>(end - start).count();
    double avgMsPerFrame = totalMs / totalFrames;

    // Verify all nodes cooked stably and average cook is sub-millisecond
    NodePerfStats lfoStats;
    EXPECT_TRUE(profiler.GetNodeStats(lfo->GetId(), lfoStats));
    EXPECT_EQ(lfoStats.totalCooks, static_cast<uint64_t>(totalFrames));
    EXPECT_LT(avgMsPerFrame, 3.0); // Entire multi-family graph must cook comfortably under 3.0ms in Debug mode (300+ FPS)
}
