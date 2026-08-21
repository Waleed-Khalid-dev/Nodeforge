#include <gtest/gtest.h>
#include "physics/PhysicsWorld.h"
#include "physics/CollisionPrimitives.h"
#include "operators/comp/PhysicsSolverComp.h"
#include "operators/geom/RigidBodyGeomOp.h"
#include "operators/geom/ColliderGeomOp.h"
#include "operators/chan/PhysicsForceChanOp.h"
#include "project/ProjectSerializer.h"
#include "graph/CoreNodes.h"
#include <filesystem>

using namespace nf;
using namespace nf::project;

class PhysicsSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        RegisterCoreNodes(NodeRegistry::Instance());
    }
};

TEST_F(PhysicsSystemTest, CollisionPrimitivesSphereSphere) {
    auto manifold = CollisionPrimitives::TestSphereSphere(
        glm::vec3(0.0f, 0.0f, 0.0f), 1.0f,
        glm::vec3(1.5f, 0.0f, 0.0f), 1.0f);

    EXPECT_TRUE(manifold.hasCollision);
    EXPECT_NEAR(manifold.penetration, 0.5f, 0.001f);
    EXPECT_NEAR(manifold.normal.x, -1.0f, 0.01f);
}

TEST_F(PhysicsSystemTest, CollisionPrimitivesBoxPlane) {
    glm::quat rot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    auto manifold = CollisionPrimitives::TestBoxPlane(
        glm::vec3(0.0f, 0.4f, 0.0f), glm::vec3(1.0f), rot,
        glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    EXPECT_TRUE(manifold.hasCollision);
    EXPECT_NEAR(manifold.penetration, 0.1f, 0.01f);
}

TEST_F(PhysicsSystemTest, CollisionPrimitivesBoxBox) {
    glm::quat rot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    auto manifold = CollisionPrimitives::TestBoxBox(
        glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(2.0f), rot,
        glm::vec3(1.5f, 0.0f, 0.0f), glm::vec3(2.0f), rot);

    EXPECT_TRUE(manifold.hasCollision);
    EXPECT_NEAR(manifold.penetration, 0.5f, 0.01f);
}

TEST_F(PhysicsSystemTest, PhysicsWorldGravityAndRestitution) {
    PhysicsWorld world;
    world.SetGravity(glm::vec3(0.0f, -10.0f, 0.0f));

    RigidBody ball;
    ball.shape = ShapeType::Sphere;
    ball.size = glm::vec3(1.0f);
    ball.position = glm::vec3(0.0f, 5.0f, 0.0f);
    ball.restitution = 0.5f;
    uint32_t id = world.AddBody(ball);

    RigidBody ground;
    ground.type = BodyType::Static;
    ground.shape = ShapeType::Plane;
    ground.position = glm::vec3(0.0f, 0.0f, 0.0f);
    ground.size = glm::vec3(0.0f, 1.0f, 0.0f);
    world.AddBody(ground);

    // Step 1 second
    for (int i = 0; i < 60; ++i) {
        world.Step(1.0f / 60.0f, 4);
    }

    auto* b = world.GetBody(id);
    ASSERT_NE(b, nullptr);
    EXPECT_GE(b->position.y, 0.9f); // Kept above ground plane (radius 1.0)
}

TEST_F(PhysicsSystemTest, RigidBodyGeomOpCooking) {
    auto rb = std::make_unique<RigidBodyGeomOp>(1, "TestBody");
    rb->SetParam("initial_pos", glm::vec3(0.0f, 10.0f, 0.0f));

    CookContext ctx{};
    ctx.deltaTimeSeconds = 1.0 / 60.0;
    EXPECT_TRUE(rb->Cook(ctx));

    const auto& outVal = rb->GetOutputPin("output")->GetValue();
    ASSERT_TRUE(outVal.Is<GeometryData>());
    const auto& geom = outVal.Get<GeometryData>();
    EXPECT_GT(geom.GetVertexCount(), 0u);
}

TEST_F(PhysicsSystemTest, LoadKineticPhysicsArenaSampleProject) {
    std::filesystem::path samplePath = "samples/09_kinetic_physics_arena/kinetic_physics_arena.nfp";
    ASSERT_TRUE(std::filesystem::exists(samplePath)) << "Missing file: " << samplePath;

    ProjectData proj;
    std::string err;
    bool success = ProjectSerializer::LoadFromFile(samplePath.string(), proj, &err);
    ASSERT_TRUE(success) << "Failed loading kinetic_physics_arena.nfp: " << err;

    EXPECT_EQ(proj.metadata.name, "Kinetic Physics Arena & Domino Cascade");
    EXPECT_GE(proj.rootGraph->GetNodes().size(), 10u);
    EXPECT_GE(proj.rootGraph->GetWires().size(), 7u);

    // Verify solver and rigid body nodes exist
    bool hasSolver = false;
    bool hasRigid = false;
    for (const auto& [id, node] : proj.rootGraph->GetNodes()) {
        if (node->GetTypeName() == "PhysicsSolverComp") hasSolver = true;
        if (node->GetTypeName() == "RigidBodyGeomOp") hasRigid = true;
    }
    EXPECT_TRUE(hasSolver);
    EXPECT_TRUE(hasRigid);
}
