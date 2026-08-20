#include <gtest/gtest.h>
#include "graph/Graph.h"
#include "graph/CoreNodes.h"
#include "graph/NodeRegistry.h"
#include "operators/geom/GridGeomOp.h"
#include "operators/geom/SphereGeomOp.h"
#include "operators/geom/BoxGeomOp.h"
#include "operators/geom/TransformGeomOp.h"
#include "operators/geom/MergeGeomOp.h"
#include "operators/geom/NoiseDeformGeomOp.h"
#include "operators/geom/NormalsGeomOp.h"
#include "operators/geom/ChanToGeomOp.h"
#include "operators/mat/ConstantMatOp.h"
#include "operators/mat/PhongMatOp.h"
#include "operators/mat/GLSLMatOp.h"
#include "operators/comp/CameraComp.h"
#include "operators/comp/LightComp.h"
#include "operators/comp/GeometryComp.h"
#include "operators/tex/RenderTexOp.h"
#include "operators/chan/ConstantChanOp.h"
#include "operators/data/TableDataOp.h"

using namespace nf;

class GeomOpPipelineTest : public ::testing::Test {
protected:
    void SetUp() override {
        RegisterCoreNodes(NodeRegistry::Instance());
    }
};

TEST_F(GeomOpPipelineTest, PrimitivesAndModifiersCook) {
    Graph graph;
    auto* sphere = graph.CreateNode<SphereGeomOp>("sphere1");
    auto* xform = graph.CreateNode<TransformGeomOp>("xform1");
    auto* noise = graph.CreateNode<NoiseDeformGeomOp>("noise1");
    auto* norm = graph.CreateNode<NormalsGeomOp>("norm1");

    sphere->SetParam("radius", 2.0f);
    xform->SetParam("translate", glm::vec3(0.0f, 1.0f, 0.0f));
    noise->SetParam("amplitude", 0.5f);
    norm->SetParam("mode", static_cast<int32_t>(0)); // Smooth

    graph.Connect(sphere->GetOutputPin("output"), xform->GetInputPin("input"));
    graph.Connect(xform->GetOutputPin("output"), noise->GetInputPin("input"));
    graph.Connect(noise->GetOutputPin("output"), norm->GetInputPin("input"));

    CookContext ctx{ .frameIndex = 1 };
    EXPECT_TRUE(graph.CookAll(ctx));

    const GeometryData* outGeom = norm->GetOutputGeometry();
    ASSERT_NE(outGeom, nullptr);
    EXPECT_FALSE(outGeom->IsEmpty());
    EXPECT_GT(outGeom->GetVertexCount(), 100);
}

TEST_F(GeomOpPipelineTest, ChanToGeomPointCloudGeneration) {
    Graph graph;
    auto* constChan = graph.CreateNode<ConstantChanOp>("constChan1");
    auto* chanToGeom = graph.CreateNode<ChanToGeomOp>("chanToGeom1");

    constChan->SetParam("channel_names", std::string("tx ty tz"));
    constChan->SetParam("values", glm::vec4(1.0f, 2.0f, 3.0f, 0.0f));
    constChan->SetParam("sample_count", static_cast<int32_t>(10));

    graph.Connect(constChan->GetOutputPin("output"), chanToGeom->GetInputPin("input"));

    CookContext ctx{ .frameIndex = 1 };
    EXPECT_TRUE(graph.CookAll(ctx));

    const GeometryData* pts = chanToGeom->GetOutputGeometry();
    ASSERT_NE(pts, nullptr);
    EXPECT_EQ(pts->GetVertexCount(), 10);
}

TEST_F(GeomOpPipelineTest, MaterialOperatorsCook) {
    ConstantMatOp constMat(1, "constMat1");
    constMat.SetParam("color", glm::vec4(1.0f, 0.5f, 0.2f, 1.0f));
    constMat.SetParam("wireframe", true);

    CookContext ctx{ .frameIndex = 1 };
    EXPECT_TRUE(constMat.Cook(ctx));
    EXPECT_EQ(constMat.GetMaterialData().type, MaterialType::Constant);
    EXPECT_TRUE(constMat.GetMaterialData().wireframe);

    PhongMatOp phongMat(2, "phongMat1");
    phongMat.SetParam("diffuse_color", glm::vec4(0.0f, 0.8f, 1.0f, 1.0f));
    phongMat.SetParam("shininess", 64.0f);
    EXPECT_TRUE(phongMat.Cook(ctx));
    EXPECT_EQ(phongMat.GetMaterialData().type, MaterialType::Phong);
    EXPECT_FLOAT_EQ(phongMat.GetMaterialData().shininess, 64.0f);

    GLSLMatOp glslMat(3, "glslMat1");
    EXPECT_TRUE(glslMat.Cook(ctx));
    EXPECT_EQ(glslMat.GetMaterialData().type, MaterialType::GLSL);
}

TEST_F(GeomOpPipelineTest, CameraAndLightComps) {
    CameraComp cam(4, "cam1");
    cam.SetParam("position", glm::vec3(0, 2, 10));
    cam.SetParam("look_at", glm::vec3(0, 0, 0));
    cam.SetParam("fov_y", 60.0f);

    CookContext ctx{ .frameIndex = 1 };
    EXPECT_TRUE(cam.Cook(ctx));
    glm::mat4 view = cam.GetViewMatrix();
    EXPECT_NE(view[3][2], 0.0f);
    glm::mat4 proj = cam.GetProjectionMatrix(16.0f / 9.0f);
    EXPECT_NE(proj[0][0], 0.0f);

    LightComp light(5, "light1");
    light.SetParam("intensity", 2.5f);
    EXPECT_TRUE(light.Cook(ctx));
    EXPECT_FLOAT_EQ(light.GetLightData().intensity, 2.5f);
}

TEST_F(GeomOpPipelineTest, GeometryCompInstancingFromDataAndChan) {
    Graph graph;
    auto* box = graph.CreateNode<BoxGeomOp>("box1");
    auto* mat = graph.CreateNode<PhongMatOp>("mat1");
    auto* table = graph.CreateNode<TableDataOp>("table1");
    auto* geoComp = graph.CreateNode<GeometryComp>("geoComp1");

    table->SetParam("raw_csv", std::string("tx,ty,tz\n0,0,0\n5,0,0\n10,0,0"));
    geoComp->SetParam("enable_instancing", true);

    graph.Connect(box->GetOutputPin("output"), geoComp->GetInputPin("geometry"));
    graph.Connect(mat->GetOutputPin("output"), geoComp->GetInputPin("material"));
    graph.Connect(table->GetOutputPin("output"), geoComp->GetInputPin("instances"));

    CookContext ctx{ .frameIndex = 1 };
    EXPECT_TRUE(graph.CookAll(ctx));

    auto insts = geoComp->GetInstanceTransforms();
    EXPECT_EQ(insts.size(), 3);
}

TEST_F(GeomOpPipelineTest, RenderTexOpSceneEvaluation) {
    Graph graph;
    auto* grid = graph.CreateNode<GridGeomOp>("grid1");
    auto* cam = graph.CreateNode<CameraComp>("cam1");
    auto* light = graph.CreateNode<LightComp>("light1");
    auto* render = graph.CreateNode<RenderTexOp>("render1");

    graph.Connect(grid->GetOutputPin("output"), render->GetInputPin("geo1"));
    graph.Connect(cam->GetOutputPin("output"), render->GetInputPin("camera"));
    graph.Connect(light->GetOutputPin("output"), render->GetInputPin("light"));

    render->SetParam("resolution", glm::vec2(640.0f, 360.0f));

    CookContext ctx{ .frameIndex = 1 };
    EXPECT_TRUE(graph.CookAll(ctx));
}
