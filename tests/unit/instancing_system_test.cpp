#include <gtest/gtest.h>
#include "operators/geom/InstanceGeomOp.h"
#include "operators/geom/SphereGeomOp.h"
#include "operators/geom/BoxGeomOp.h"
#include "core/DataTable.h"
#include "core/ChannelBuffer.h"
#include "project/ProjectSerializer.h"
#include "graph/CoreNodes.h"
#include <filesystem>

using namespace nf;
using namespace nf::project;

class InstancingSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        RegisterCoreNodes(NodeRegistry::Instance());
    }
};

TEST_F(InstancingSystemTest, GridArrayDistribution) {
    auto instancer = std::make_unique<InstanceGeomOp>(1, "Instancer1");
    instancer->SetParam("distribution_mode", static_cast<int32_t>(2)); // Grid Array
    instancer->SetParam("grid_count", glm::vec3(10.0f, 10.0f, 2.0f));          // 200 instances
    instancer->SetParam("grid_spacing", glm::vec3(2.0f, 2.0f, 2.0f));

    CookContext ctx{};
    EXPECT_TRUE(instancer->Cook(ctx));

    const GeometryData* geom = instancer->GetOutputGeometry();
    ASSERT_NE(geom, nullptr);
    EXPECT_EQ(geom->GetInstanceCount(), 200u);
}

TEST_F(InstancingSystemTest, FibonacciSpiralDistribution) {
    auto instancer = std::make_unique<InstanceGeomOp>(2, "SpiralInstancer");
    instancer->SetParam("distribution_mode", static_cast<int32_t>(3)); // Fibonacci Spiral
    instancer->SetParam("spiral_count", static_cast<int32_t>(1500));
    instancer->SetParam("spiral_radius_scale", 0.8f);

    CookContext ctx{};
    EXPECT_TRUE(instancer->Cook(ctx));

    const GeometryData* geom = instancer->GetOutputGeometry();
    ASSERT_NE(geom, nullptr);
    EXPECT_EQ(geom->GetInstanceCount(), 1500u);
}

TEST_F(InstancingSystemTest, MeshSurfaceAndNormalAlignment) {
    auto sphere = std::make_unique<SphereGeomOp>(3, "TargetSphere");
    sphere->SetParam("rows", static_cast<int32_t>(8));
    sphere->SetParam("cols", static_cast<int32_t>(8));

    auto instancer = std::make_unique<InstanceGeomOp>(4, "SurfaceInstancer");
    instancer->SetParam("distribution_mode", static_cast<int32_t>(1)); // Mesh Surface
    instancer->SetParam("align_to_normals", true);

    CookContext ctx{};
    sphere->Cook(ctx);

    instancer->GetInputPin("distribution_mesh")->SetValue(PinValue(*sphere->GetOutputGeometry()));
    EXPECT_TRUE(instancer->Cook(ctx));

    const GeometryData* geom = instancer->GetOutputGeometry();
    ASSERT_NE(geom, nullptr);
    EXPECT_EQ(geom->GetInstanceCount(), sphere->GetOutputGeometry()->GetVertexCount());
}

TEST_F(InstancingSystemTest, TableAndChannelBinding) {
    auto instancer = std::make_unique<InstanceGeomOp>(5, "TableInstancer");
    instancer->SetParam("distribution_mode", static_cast<int32_t>(0)); // Table/Chan

    // Create 3-point DataTable
    DataTable table(3, 3);
    table.SetCell(0, 0, "1.0"); table.SetCell(0, 1, "2.0"); table.SetCell(0, 2, "3.0");
    table.SetCell(1, 0, "4.0"); table.SetCell(1, 1, "5.0"); table.SetCell(1, 2, "6.0");
    table.SetCell(2, 0, "7.0"); table.SetCell(2, 1, "8.0"); table.SetCell(2, 2, "9.0");

    instancer->GetInputPin("instances_data")->SetValue(PinValue(table));

    CookContext ctx{};
    EXPECT_TRUE(instancer->Cook(ctx));

    const GeometryData* geom = instancer->GetOutputGeometry();
    ASSERT_NE(geom, nullptr);
    EXPECT_EQ(geom->GetInstanceCount(), 3u);
}

TEST_F(InstancingSystemTest, LoadMatrixInstancingSampleProject) {
    std::filesystem::path samplePath = "samples/06_matrix_instancing/matrix_instancing.nfp";
    ASSERT_TRUE(std::filesystem::exists(samplePath)) << "Missing file: " << samplePath;

    ProjectData proj;
    std::string err;
    bool success = ProjectSerializer::LoadFromFile(samplePath.string(), proj, &err);
    ASSERT_TRUE(success) << "Failed loading matrix_instancing.nfp: " << err;

    EXPECT_EQ(proj.metadata.name, "Audio-Reactive Kinetic Matrix Instancing");
    EXPECT_GE(proj.rootGraph->GetNodes().size(), 11u);
    EXPECT_GE(proj.rootGraph->GetWires().size(), 11u);

    // Verify InstanceGeomOp exists
    bool hasInstancer = false;
    for (const auto& [id, node] : proj.rootGraph->GetNodes()) {
        if (node->GetTypeName() == "InstanceGeomOp") {
            hasInstancer = true;
            break;
        }
    }
    EXPECT_TRUE(hasInstancer);
}
