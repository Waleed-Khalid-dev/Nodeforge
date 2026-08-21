#include <gtest/gtest.h>
#include "render/VolumetricMath.h"
#include "operators/tex/VolumetricCloudTexOp.h"
#include "operators/tex/LightShaftTexOp.h"
#include "operators/mat/VolumetricFogMatOp.h"
#include "operators/geom/VoxelGridGeomOp.h"
#include "project/ProjectSerializer.h"
#include "graph/CoreNodes.h"
#include <filesystem>

using namespace nf;
using namespace nf::project;

class VolumetricSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        RegisterCoreNodes(NodeRegistry::Instance());
    }
};

TEST_F(VolumetricSystemTest, BeerLambertTransmittanceExtinction) {
    float extinction = 0.5f;
    float density = 2.0f;
    float step = 1.0f;

    float t0 = VolumetricMath::BeerLambert(extinction, density, 0.0f);
    EXPECT_NEAR(t0, 1.0f, 0.0001f);

    float t1 = VolumetricMath::BeerLambert(extinction, density, step);
    EXPECT_NEAR(t1, std::exp(-1.0f), 0.001f);

    float t2 = VolumetricMath::BeerLambert(extinction, density, step * 2.0f);
    EXPECT_NEAR(t1 * t1, t2, 0.001f);
}

TEST_F(VolumetricSystemTest, HenyeyGreensteinPhaseFunction) {
    float gForward = 0.6f;
    float phaseForward = VolumetricMath::HenyeyGreenstein(1.0f, gForward);  // Forward angle (0 deg)
    float phaseBackward = VolumetricMath::HenyeyGreenstein(-1.0f, gForward); // Backward angle (180 deg)

    EXPECT_GT(phaseForward, phaseBackward);
    EXPECT_GT(phaseForward, 0.0f);
    EXPECT_GT(phaseBackward, 0.0f);
}

TEST_F(VolumetricSystemTest, Procedural3DNoiseAndDensity) {
    glm::vec3 pos(2.5f, 4.0f, 1.2f);
    float simplex = VolumetricMath::SimplexNoise3D(pos);
    EXPECT_GE(simplex, -1.0f);
    EXPECT_LE(simplex, 1.0f);

    float worley = VolumetricMath::WorleyNoise3D(pos);
    EXPECT_GE(worley, 0.0f);

    float density = VolumetricMath::SampleDensity(pos, 1.0f, 0.5f);
    EXPECT_GE(density, 0.0f);
}

TEST_F(VolumetricSystemTest, VolumetricCloudTexOpCooking) {
    auto cloud = std::make_unique<VolumetricCloudTexOp>(1, "TestClouds");
    cloud->SetParam("density", 1.5f);
    cloud->SetParam("step_count", static_cast<int32_t>(16));

    CookContext ctx{};
    ctx.timeSeconds = 1.0;
    EXPECT_TRUE(cloud->Cook(ctx));

    EXPECT_NEAR(cloud->GetDensity(), 1.5f, 0.001f);
    EXPECT_EQ(cloud->GetStepCount(), 16);
}

TEST_F(VolumetricSystemTest, VoxelGridGeomOpCooking) {
    auto voxel = std::make_unique<VoxelGridGeomOp>(2, "TestVoxels");
    voxel->SetParam("box_size", glm::vec3(20.0f, 10.0f, 20.0f));

    CookContext ctx{};
    EXPECT_TRUE(voxel->Cook(ctx));

    const auto& outVal = voxel->GetOutputPin("output")->GetValue();
    ASSERT_TRUE(outVal.Is<GeometryData>());
    const auto& geom = outVal.Get<GeometryData>();

    EXPECT_GT(geom.GetVertexCount(), 0u);
}

TEST_F(VolumetricSystemTest, LoadVolumetricNebulaCathedralSampleProject) {
    std::filesystem::path samplePath = "samples/11_volumetric_nebula_cathedral/volumetric_nebula_cathedral.nfp";
    ASSERT_TRUE(std::filesystem::exists(samplePath)) << "Missing file: " << samplePath;

    ProjectData proj;
    std::string err;
    bool success = ProjectSerializer::LoadFromFile(samplePath.string(), proj, &err);
    ASSERT_TRUE(success) << "Failed loading volumetric_nebula_cathedral.nfp: " << err;

    EXPECT_EQ(proj.metadata.name, "Interactive Volumetric Nebula & Cosmic Cathedral");
    EXPECT_GE(proj.rootGraph->GetNodes().size(), 8u);
    EXPECT_GE(proj.rootGraph->GetWires().size(), 5u);

    bool hasClouds = false;
    bool hasGodRays = false;
    for (const auto& [id, node] : proj.rootGraph->GetNodes()) {
        if (node->GetTypeName() == "VolumetricCloudTexOp") hasClouds = true;
        if (node->GetTypeName() == "LightShaftTexOp") hasGodRays = true;
    }
    EXPECT_TRUE(hasClouds);
    EXPECT_TRUE(hasGodRays);
}
