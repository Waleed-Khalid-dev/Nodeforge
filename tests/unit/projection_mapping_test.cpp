#include <gtest/gtest.h>
#include "graph/Graph.h"
#include "graph/CoreNodes.h"
#include "graph/NodeRegistry.h"
#include "media/DisplayManager.h"
#include "operators/tex/MovieFileInTexOp.h"
#include "operators/tex/VideoDeviceInTexOp.h"
#include "operators/tex/SpoutInTexOp.h"
#include "operators/tex/SpoutOutTexOp.h"
#include "operators/tex/NDIInTexOp.h"
#include "operators/tex/NDIOutTexOp.h"
#include "operators/tex/ProjectorOutTexOp.h"
#include "operators/tex/WarpBlendTexOp.h"

using namespace nf;

class ProjectionMappingTest : public ::testing::Test {
protected:
    void SetUp() override {
        RegisterCoreNodes(NodeRegistry::Instance());
    }
};

TEST_F(ProjectionMappingTest, DisplayManagerEnumerationAndWindows) {
    auto& dm = DisplayManager::Instance();
    auto displays = dm.EnumerateDisplays();
    EXPECT_FALSE(displays.empty());

    OutputWindow* win1 = dm.GetOrCreateWindow(1, "Projector 1", false);
    ASSERT_NE(win1, nullptr);
    EXPECT_EQ(win1->GetDisplayIndex(), 1);
    EXPECT_TRUE(dm.HasWindow(1));

    dm.CloseWindow(1);
    EXPECT_FALSE(dm.HasWindow(1));
}

TEST_F(ProjectionMappingTest, MovieToWarpToProjectorPipeline) {
    Graph graph;
    auto* movie = graph.CreateNode<MovieFileInTexOp>("movie1");
    auto* warp = graph.CreateNode<WarpBlendTexOp>("warp1");
    auto* proj = graph.CreateNode<ProjectorOutTexOp>("proj1");

    movie->SetParam("file_path", std::string("test_movie.mp4"));
    warp->SetParam("blend_right", 0.15f);
    warp->SetParam("blend_gamma", 2.2f);
    proj->SetParam("display_index", static_cast<int32_t>(1));

    graph.Connect(movie->GetOutputPin("output"), warp->GetInputPin("input"));
    graph.Connect(warp->GetOutputPin("output"), proj->GetInputPin("input"));

    CookContext ctx{ .frameIndex = 1, .timeSeconds = 0.0 };
    EXPECT_TRUE(graph.CookAll(ctx));
}

TEST_F(ProjectionMappingTest, SpoutAndNDIInteropNodes) {
    Graph graph;
    auto* spoutIn = graph.CreateNode<SpoutInTexOp>("spoutIn1");
    auto* spoutOut = graph.CreateNode<SpoutOutTexOp>("spoutOut1");
    auto* ndiIn = graph.CreateNode<NDIInTexOp>("ndiIn1");
    auto* ndiOut = graph.CreateNode<NDIOutTexOp>("ndiOut1");
    auto* cam = graph.CreateNode<VideoDeviceInTexOp>("cam1");
    auto* camOut = graph.CreateNode<SpoutOutTexOp>("camOut1");

    graph.Connect(spoutIn->GetOutputPin("output"), spoutOut->GetInputPin("input"));
    graph.Connect(ndiIn->GetOutputPin("output"), ndiOut->GetInputPin("input"));
    graph.Connect(cam->GetOutputPin("output"), camOut->GetInputPin("input"));

    CookContext ctx{ .frameIndex = 1 };
    EXPECT_TRUE(graph.CookAll(ctx));
}
