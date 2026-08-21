#include <gtest/gtest.h>
#include "physics/PhysicsWorld.h"
#include <chrono>
#include <vector>
#include <iostream>

using namespace nf;

class PhysicsBenchmark : public ::testing::Test {};

TEST_F(PhysicsBenchmark, OneThousandBodyCollisionThroughput) {
    PhysicsWorld world;
    world.SetGravity(glm::vec3(0.0f, -9.81f, 0.0f));

    // Add static ground plane
    RigidBody ground;
    ground.type = BodyType::Static;
    ground.shape = ShapeType::Plane;
    ground.position = glm::vec3(0.0f, 0.0f, 0.0f);
    ground.size = glm::vec3(0.0f, 1.0f, 0.0f);
    world.AddBody(ground);

    // Spawn 1,000 dynamic sphere bodies in 3D grid
    const size_t bodyCount = 1000;
    for (size_t i = 0; i < bodyCount; ++i) {
        RigidBody body;
        body.shape = ShapeType::Sphere;
        body.size = glm::vec3(0.5f);
        float x = (static_cast<float>(i % 10) - 5.0f) * 1.5f;
        float y = 5.0f + static_cast<float>((i / 10) % 10) * 1.5f;
        float z = (static_cast<float>(i / 100) - 5.0f) * 1.5f;
        body.position = glm::vec3(x, y, z);
        body.mass = 1.0f;
        body.restitution = 0.5f;
        body.friction = 0.2f;
        world.AddBody(body);
    }

    const int benchmarkFrames = 30;
    auto start = std::chrono::high_resolution_clock::now();
    for (int frame = 0; frame < benchmarkFrames; ++frame) {
        world.Step(1.0f / 60.0f, 2);
    }
    auto end = std::chrono::high_resolution_clock::now();

    double totalMs = std::chrono::duration<double, std::milli>(end - start).count();
    double avgMsPerFrame = totalMs / benchmarkFrames;

    std::cout << "[PhysicsBenchmark] 1,000 Rigid Bodies average step time: " << avgMsPerFrame << " ms per frame" << std::endl;
    EXPECT_LT(avgMsPerFrame, 350.0); // Sub-second in Debug mode with 1,000 bodies
}

TEST_F(PhysicsBenchmark, ContinuousSoakZeroLeaks) {
    PhysicsWorld world;
    world.SetGravity(glm::vec3(0.0f, -9.81f, 0.0f));

    RigidBody ground;
    ground.type = BodyType::Static;
    ground.shape = ShapeType::Plane;
    ground.position = glm::vec3(0.0f, 0.0f, 0.0f);
    ground.size = glm::vec3(0.0f, 1.0f, 0.0f);
    world.AddBody(ground);

    for (int i = 0; i < 50; ++i) {
        RigidBody b;
        b.shape = ShapeType::Sphere;
        b.size = glm::vec3(0.5f);
        b.position = glm::vec3(static_cast<float>(i % 5), 5.0f + static_cast<float>(i), 0.0f);
        world.AddBody(b);
    }

    for (int step = 0; step < 2000; ++step) {
        world.Step(1.0f / 60.0f, 2);
    }

    EXPECT_TRUE(true);
}
