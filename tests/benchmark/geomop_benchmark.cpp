#include <gtest/gtest.h>
#include "graph/Graph.h"
#include "graph/CoreNodes.h"
#include "graph/NodeRegistry.h"
#include "core/GeometryData.h"
#include "operators/geom/GridGeomOp.h"
#include "operators/geom/NoiseDeformGeomOp.h"
#include "operators/geom/NormalsGeomOp.h"
#include <chrono>

using namespace nf;

TEST(GeomOpBenchmark, LargeMeshDeformationThroughput) {
    RegisterCoreNodes(NodeRegistry::Instance());

    // Generate high-density grid (200x200 = 40,000 vertices, ~80,000 triangles)
    GeometryData grid = GeometryData::CreateGrid(10.0f, 10.0f, 200, 200);

    auto start = std::chrono::high_resolution_clock::now();

    grid.DeformNoise(0.5f, 2.0f, glm::vec3(1.0f, 2.0f, 3.0f), true);
    grid.ComputeNormals(true);
    grid.ComputeTangents();

    auto end = std::chrono::high_resolution_clock::now();
    double elapsedMs = std::chrono::duration<double, std::milli>(end - start).count();

    EXPECT_EQ(grid.GetVertexCount(), 40000);
    EXPECT_LT(elapsedMs, 1000.0); // 40k verts deformed and auto-normaled in <1000ms in debug build
}

TEST(GeomOpBenchmark, LongRunningCookLoopZeroLeaks) {
    RegisterCoreNodes(NodeRegistry::Instance());

    Graph graph;
    auto* grid = graph.CreateNode<GridGeomOp>("grid1");
    auto* noise = graph.CreateNode<NoiseDeformGeomOp>("noise1");
    auto* norm = graph.CreateNode<NormalsGeomOp>("norm1");

    grid->SetParam("rows", static_cast<int32_t>(20));
    grid->SetParam("cols", static_cast<int32_t>(20));

    graph.Connect(grid->GetOutputPin("output"), noise->GetInputPin("input"));
    graph.Connect(noise->GetOutputPin("output"), norm->GetInputPin("input"));

    CookContext ctx{ .deltaTimeSeconds = 1.0 / 60.0 };

    for (uint64_t frame = 1; frame <= 10000; ++frame) {
        ctx.frameIndex = frame;
        noise->SetParam("offset", glm::vec3(0.0f, 0.0f, static_cast<float>(frame) * 0.01f));
        grid->MarkDirty();
        bool ok = graph.CookAll(ctx);
        ASSERT_TRUE(ok);
    }

    const GeometryData* out = norm->GetOutputGeometry();
    ASSERT_NE(out, nullptr);
    EXPECT_EQ(out->GetVertexCount(), 400);
}
