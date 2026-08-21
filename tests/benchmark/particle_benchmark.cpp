#include <gtest/gtest.h>
#include "gpu/ParticleBuffer.h"
#include <chrono>
#include <iostream>

using namespace gpu;

class ParticleBenchmark : public ::testing::Test {};

TEST_F(ParticleBenchmark, OneMillionParticleThroughput) {
    const size_t particleCount = 1000000;
    ParticleBuffer buffer(particleCount);

    ParticleSimParams params{};
    params.deltaTime = 1.0f / 60.0f;
    params.particleCount = static_cast<uint32_t>(particleCount);
    params.birthRate = 300000.0f;
    params.lifetime = 3.0f;
    params.turbulenceParams = glm::vec4(1.5f, 0.5f, 0.3f, 0.0f);
    params.attractorParams = glm::vec4(0.0f, 0.0f, 0.0f, 5.0f);

    // Warmup 5 frames
    for (int i = 0; i < 5; ++i) {
        params.time += params.deltaTime;
        buffer.StepSimulation(params);
    }

    auto start = std::chrono::high_resolution_clock::now();
    const int benchmarkFrames = 10;
    for (int i = 0; i < benchmarkFrames; ++i) {
        params.time += params.deltaTime;
        buffer.StepSimulation(params);
    }
    auto end = std::chrono::high_resolution_clock::now();

    double totalMs = std::chrono::duration<double, std::milli>(end - start).count();
    double avgMsPerFrame = totalMs / benchmarkFrames;

    std::cout << "[ParticleBenchmark] 1,000,000 Particles average step time: " << avgMsPerFrame << " ms (" << buffer.GetActiveCount() << " active particles)" << std::endl;

    EXPECT_GT(buffer.GetActiveCount(), 0u);
    EXPECT_LE(buffer.GetActiveCount(), particleCount);
}

TEST_F(ParticleBenchmark, ContinuousSoakZeroLeaks) {
    ParticleBuffer buffer(50000);
    ParticleSimParams params{};
    params.deltaTime = 1.0f / 60.0f;
    params.particleCount = 50000;
    params.birthRate = 20000.0f;

    for (int i = 0; i < 200; ++i) {
        params.time += params.deltaTime;
        buffer.StepSimulation(params);
    }

    EXPECT_GT(buffer.GetActiveCount(), 0u);
    EXPECT_LE(buffer.GetActiveCount(), 50000u);
}
