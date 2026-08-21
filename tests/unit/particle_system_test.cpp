#include <gtest/gtest.h>
#include "gpu/ParticleBuffer.h"
#include "operators/geom/ParticleEmitterGeomOp.h"
#include "operators/geom/ParticleForceGeomOp.h"
#include "operators/geom/ParticleAttractorGeomOp.h"
#include "operators/mat/ParticleMatOp.h"
#include "project/ProjectSerializer.h"
#include "graph/CoreNodes.h"
#include <filesystem>

using namespace nf;
using namespace nf::project;
using namespace gpu;

class ParticleSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        RegisterCoreNodes(NodeRegistry::Instance());
    }
};

TEST_F(ParticleSystemTest, ParticleBufferLifecycleAndRecycling) {
    ParticleBuffer buffer(1000);
    EXPECT_EQ(buffer.GetCapacity(), 1000u);
    EXPECT_EQ(buffer.GetActiveCount(), 0u);

    ParticleSimParams params{};
    params.deltaTime = 1.0f / 60.0f;
    params.particleCount = 1000;
    params.birthRate = 6000.0f; // Spawns 100 per frame
    params.lifetime = 0.5f;

    // Step 10 frames -> 1000 particles spawned
    for (int f = 0; f < 10; ++f) {
        params.time += params.deltaTime;
        buffer.StepSimulation(params);
    }

    EXPECT_GT(buffer.GetActiveCount(), 0u);
    EXPECT_LE(buffer.GetActiveCount(), 1000u);

    // Convert to GeometryData
    GeometryData geom = buffer.ToGeometryData();
    EXPECT_EQ(geom.GetVertexCount(), buffer.GetActiveCount());
    EXPECT_EQ(geom.GetIndexCount(), buffer.GetActiveCount());
}

TEST_F(ParticleSystemTest, ParticleEmitterGeomOpCooking) {
    auto emitter = std::make_unique<ParticleEmitterGeomOp>(1, "Emitter1");
    emitter->SetParam("birth_rate", 3000.0f);
    emitter->SetParam("lifetime", 2.0f);
    emitter->SetParam("initial_speed", 5.0f);

    CookContext ctx{};
    ctx.deltaTimeSeconds = 1.0 / 60.0;
    ctx.timeSeconds = 0.0;
    ctx.frameIndex = 1;

    EXPECT_TRUE(emitter->Cook(ctx));
    const GeometryData* geom = emitter->GetOutputGeometry();
    ASSERT_NE(geom, nullptr);
    EXPECT_GT(geom->GetVertexCount(), 0u);
}

TEST_F(ParticleSystemTest, ParticleForceGeomOpIntegration) {
    auto emitter = std::make_unique<ParticleEmitterGeomOp>(1, "Emitter1");
    auto force = std::make_unique<ParticleForceGeomOp>(2, "Force1");

    force->SetParam("gravity", glm::vec3(0.0f, -9.81f, 0.0f));
    force->SetParam("turbulence_strength", 2.0f);

    CookContext ctx{};
    ctx.deltaTimeSeconds = 1.0 / 60.0;
    ctx.timeSeconds = 0.1;

    emitter->Cook(ctx);
    force->GetInputPin("input")->SetValue(PinValue(*emitter->GetOutputGeometry()));
    EXPECT_TRUE(force->Cook(ctx));

    const GeometryData* outGeom = force->GetOutputGeometry();
    ASSERT_NE(outGeom, nullptr);
    EXPECT_EQ(outGeom->GetVertexCount(), emitter->GetOutputGeometry()->GetVertexCount());
}

TEST_F(ParticleSystemTest, ParticleAttractorGeomOpPull) {
    auto attractor = std::make_unique<ParticleAttractorGeomOp>(3, "Attractor1");
    attractor->SetParam("position", glm::vec3(5.0f, 0.0f, 0.0f));
    attractor->SetParam("strength", 10.0f);
    attractor->SetParam("radius", 10.0f);

    // Provide test particle at origin
    std::vector<Vertex> verts;
    Vertex v{};
    v.pos = glm::vec3(0.0f, 0.0f, 0.0f);
    verts.push_back(v);
    GeometryData inGeom(std::move(verts), {0});

    attractor->GetInputPin("input")->SetValue(PinValue(inGeom));

    CookContext ctx{};
    ctx.deltaTimeSeconds = 0.1;
    EXPECT_TRUE(attractor->Cook(ctx));

    const GeometryData* outGeom = attractor->GetOutputGeometry();
    ASSERT_NE(outGeom, nullptr);
    ASSERT_EQ(outGeom->GetVertexCount(), 1u);
    // Particle at (0,0,0) should have moved towards target at (5,0,0) -> pos.x > 0
    EXPECT_GT(outGeom->GetVertices()[0].pos.x, 0.0f);
}

TEST_F(ParticleSystemTest, ParticleMatOpDescriptor) {
    auto mat = std::make_unique<ParticleMatOp>(4, "PartMat1");
    mat->SetParam("blend_mode", static_cast<int32_t>(0)); // Additive
    mat->SetParam("base_size", 0.15f);

    CookContext ctx{};
    EXPECT_TRUE(mat->Cook(ctx));
    EXPECT_TRUE(mat->GetMaterialData().useVertexColor);
}

TEST_F(ParticleSystemTest, LoadHolographicParticlesSampleProject) {
    std::filesystem::path samplePath = "samples/05_holographic_particles/holographic_particles.nfp";
    ASSERT_TRUE(std::filesystem::exists(samplePath)) << "Missing file: " << samplePath;

    ProjectData proj;
    std::string err;
    bool success = ProjectSerializer::LoadFromFile(samplePath.string(), proj, &err);
    ASSERT_TRUE(success) << "Failed loading holographic_particles.nfp: " << err;

    EXPECT_EQ(proj.metadata.name, "Holographic Interactive Particle Swarm");
    EXPECT_GE(proj.rootGraph->GetNodes().size(), 11u);
    EXPECT_GE(proj.rootGraph->GetWires().size(), 11u);

    // Verify particle nodes exist
    bool hasEmitter = false;
    bool hasForce = false;
    bool hasAttractor = false;
    bool hasMat = false;

    for (const auto& [id, node] : proj.rootGraph->GetNodes()) {
        if (node->GetTypeName() == "ParticleEmitterGeomOp") hasEmitter = true;
        if (node->GetTypeName() == "ParticleForceGeomOp") hasForce = true;
        if (node->GetTypeName() == "ParticleAttractorGeomOp") hasAttractor = true;
        if (node->GetTypeName() == "ParticleMatOp") hasMat = true;
    }

    EXPECT_TRUE(hasEmitter);
    EXPECT_TRUE(hasForce);
    EXPECT_TRUE(hasAttractor);
    EXPECT_TRUE(hasMat);
}
