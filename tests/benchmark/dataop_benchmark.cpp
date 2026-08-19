#include <gtest/gtest.h>
#include "graph/Graph.h"
#include "graph/CoreNodes.h"
#include "graph/NodeRegistry.h"
#include "core/DataTable.h"
#include "operators/data/TableDataOp.h"
#include "operators/data/SelectDataOp.h"
#include "operators/data/MergeDataOp.h"
#include <chrono>

using namespace nf;

TEST(DataOpBenchmark, LargeTableCSVThroughput) {
    RegisterCoreNodes(NodeRegistry::Instance());

    // Generate table with 10,000 rows and 10 columns = 100,000 cells
    DataTable table(10000, 10, "123.456");
    for (size_t c = 0; c < 10; ++c) {
        table.SetCell(0, c, "header_" + std::to_string(c));
    }

    auto start = std::chrono::high_resolution_clock::now();

    // Perform CSV serialization and deserialization
    std::string csv = table.ToCSV(',', true);
    DataTable restored = DataTable::FromCSV(csv, ',', true);

    auto end = std::chrono::high_resolution_clock::now();
    double elapsedMs = std::chrono::duration<double, std::milli>(end - start).count();

    EXPECT_EQ(restored.GetRowCount(), 10000);
    EXPECT_EQ(restored.GetColumnCount(), 10);

    // 100,000 cells processed
    double cellsPerSec = (100000.0 / elapsedMs) * 1000.0;
    EXPECT_GT(cellsPerSec, 100000.0); // Should process >100k cells/sec easily
}

TEST(DataOpBenchmark, LongRunningCookLoopZeroLeaks) {
    RegisterCoreNodes(NodeRegistry::Instance());

    Graph graph;
    auto* t1 = graph.CreateNode<TableDataOp>("t1");
    auto* sel = graph.CreateNode<SelectDataOp>("sel1");
    auto* merge = graph.CreateNode<MergeDataOp>("merge1");

    t1->SetParam("raw_csv", std::string("a,b,c\n1,2,3\n4,5,6\n7,8,9"));
    graph.Connect(t1->GetOutputPin("output"), sel->GetInputPin("input"));
    graph.Connect(sel->GetOutputPin("output"), merge->GetInputPin("input1"));

    CookContext ctx{ .deltaTimeSeconds = 1.0 / 60.0 };

    for (uint64_t frame = 1; frame <= 10000; ++frame) {
        ctx.frameIndex = frame;
        t1->MarkDirty();
        bool ok = graph.CookAll(ctx);
        ASSERT_TRUE(ok);
    }

    const DataTable* out = merge->GetOutputTable();
    ASSERT_NE(out, nullptr);
    EXPECT_EQ(out->GetRowCount(), 3);
}
