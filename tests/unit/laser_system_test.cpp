#include <gtest/gtest.h>
#include "laser/LaserPoint.h"
#include "laser/LaserEngine.h"
#include "operators/geom/LaserGeomOp.h"
#include "operators/geom/LaserPatternGeomOp.h"
#include "operators/geom/LaserFileInGeomOp.h"
#include "operators/chan/LaserDACChanOp.h"
#include "project/ProjectSerializer.h"
#include "graph/CoreNodes.h"
#include <filesystem>

using namespace nf;
using namespace nf::project;

class LaserSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        RegisterCoreNodes(NodeRegistry::Instance());
    }
};

TEST_F(LaserSystemTest, LaserPointCoordinateNormalization) {
    int16_t maxCoord = LaserPoint::NormalizeCoord(1.0f);
    int16_t minCoord = LaserPoint::NormalizeCoord(-1.0f);
    int16_t zeroCoord = LaserPoint::NormalizeCoord(0.0f);

    EXPECT_EQ(maxCoord, 32767);
    EXPECT_EQ(minCoord, -32767);
    EXPECT_EQ(zeroCoord, 0);

    EXPECT_NEAR(LaserPoint::DenormalizeCoord(maxCoord), 1.0f, 0.001f);
    EXPECT_NEAR(LaserPoint::DenormalizeCoord(minCoord), -1.0f, 0.001f);
}

TEST_F(LaserSystemTest, LaserEnginePathOptimizationAndBlanking) {
    std::vector<glm::vec3> verts = {
        glm::vec3(-0.5f, -0.5f, 0.0f),
        glm::vec3(0.5f, -0.5f, 0.0f),
        glm::vec3(0.0f, 0.5f, 0.0f)
    };

    int blankDelay = 6;
    int cornerDwell = 2;
    auto points = LaserEngine::Instance().OptimizePath(verts, blankDelay, cornerDwell);

    EXPECT_GE(points.size(), verts.size() + blankDelay);

    // Initial points should be blanked
    bool hasBlanked = false;
    for (int i = 0; i < blankDelay; ++i) {
        if (points[i].isBlanked) hasBlanked = true;
    }
    EXPECT_TRUE(hasBlanked);
}

TEST_F(LaserSystemTest, LaserPatternGeneration) {
    auto lissajous = LaserEngine::Instance().GenerateLissajous(3.0f, 2.0f, 0.0f, 150, glm::vec4(0.0f, 1.0f, 0.5f, 1.0f));
    EXPECT_EQ(lissajous.size(), 150u);

    auto spirograph = LaserEngine::Instance().GenerateSpirograph(1.0f, 0.3f, 0.2f, 200, glm::vec4(1.0f, 0.0f, 0.5f, 1.0f));
    EXPECT_EQ(spirograph.size(), 200u);

    auto fan = LaserEngine::Instance().GenerateBeamFan(12, 1.0f, glm::vec4(0.0f, 0.5f, 1.0f, 1.0f));
    EXPECT_EQ(fan.size(), 24u);
}

TEST_F(LaserSystemTest, LaserGeomOpCooking) {
    auto laser = std::make_unique<LaserGeomOp>(1, "TestLaserGeom");
    laser->SetParam("blank_delay", static_cast<int32_t>(4));
    laser->SetParam("corner_dwell", static_cast<int32_t>(2));

    CookContext ctx{};
    EXPECT_TRUE(laser->Cook(ctx));

    const auto& geomVal = laser->GetOutputPin("output")->GetValue();
    ASSERT_TRUE(geomVal.Is<GeometryData>());
    const auto& geom = geomVal.Get<GeometryData>();
    EXPECT_GT(geom.GetVertexCount(), 0u);

    const auto& ptsVal = laser->GetOutputPin("out_points")->GetValue();
    ASSERT_TRUE(ptsVal.Is<ChannelBuffer>());
    const auto& ptsBuf = ptsVal.Get<ChannelBuffer>();
    EXPECT_EQ(ptsBuf.GetChannelCount(), 7u);
    EXPECT_GT(ptsBuf.GetSampleCount(), 0u);
}

TEST_F(LaserSystemTest, LaserDACChanOpCooking) {
    auto dac = std::make_unique<LaserDACChanOp>(2, "TestDAC");
    dac->SetParam("safety_shutter", true);

    CookContext ctx{};
    EXPECT_TRUE(dac->Cook(ctx));

    const auto& teleVal = dac->GetOutputPin("out_telemetry")->GetValue();
    ASSERT_TRUE(teleVal.Is<ChannelBuffer>());
    const auto& teleBuf = teleVal.Get<ChannelBuffer>();
    EXPECT_EQ(teleBuf.GetChannelCount(), 4u);
    EXPECT_NEAR(teleBuf.GetChannelData(3)[0], 1.0f, 0.01f); // Shutter open
}

TEST_F(LaserSystemTest, LoadLaserSymphonySampleProject) {
    std::filesystem::path samplePath = "samples/12_laser_symphony_spectacular/laser_symphony_spectacular.nfp";
    ASSERT_TRUE(std::filesystem::exists(samplePath)) << "Missing file: " << samplePath;

    ProjectData proj;
    std::string err;
    bool success = ProjectSerializer::LoadFromFile(samplePath.string(), proj, &err);
    ASSERT_TRUE(success) << "Failed loading laser_symphony_spectacular.nfp: " << err;

    EXPECT_EQ(proj.metadata.name, "Live Laser Concert & Kinetic Beam Symphony");
    EXPECT_GE(proj.rootGraph->GetNodes().size(), 8u);
    EXPECT_GE(proj.rootGraph->GetWires().size(), 6u);

    bool hasPattern = false;
    bool hasLaserGeom = false;
    bool hasDAC = false;
    for (const auto& [id, node] : proj.rootGraph->GetNodes()) {
        if (node->GetTypeName() == "LaserPatternGeomOp") hasPattern = true;
        if (node->GetTypeName() == "LaserGeomOp") hasLaserGeom = true;
        if (node->GetTypeName() == "LaserDACChanOp") hasDAC = true;
    }
    EXPECT_TRUE(hasPattern);
    EXPECT_TRUE(hasLaserGeom);
    EXPECT_TRUE(hasDAC);
}
