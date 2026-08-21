#pragma once

#include <vector>
#include <memory>
#include <string>
#include <glm/glm.hpp>
#include "../core/GeometryData.h"

namespace gpu {

struct alignas(16) GpuParticle {
    glm::vec4 posLife{0.0f, 0.0f, 0.0f, 0.0f};   // xyz: pos, w: normalized life [0.0 .. 1.0]
    glm::vec4 velMass{0.0f, 0.0f, 0.0f, 1.0f};   // xyz: velocity, w: inverse mass
    glm::vec4 colorSize{1.0f, 1.0f, 1.0f, 0.05f}; // rgb: color, a: point size / radius
};

struct ParticleSimParams {
    float deltaTime = 1.0f / 60.0f;
    float time = 0.0f;
    uint32_t particleCount = 100000;
    float birthRate = 5000.0f;
    float lifetime = 3.0f;
    float lifetimeVar = 0.5f;
    float initialSpeed = 2.0f;
    float spreadAngle = 25.0f;
    glm::vec4 gravityAndDrag{0.0f, -1.0f, 0.0f, 0.15f};    // xyz: gravity, w: drag
    glm::vec4 turbulenceParams{1.5f, 0.5f, 0.3f, 0.0f};    // x: strength, y: freq, z: speed, w: vortex
    glm::vec4 attractorParams{0.0f, 0.0f, 0.0f, 0.0f};     // xyz: attractorPos, w: strength
    glm::vec4 attractorSettings{4.0f, 0.0f, 1.0f, 20.0f};   // x: radius, y: type (0: attract, 1: repel), z: falloff, w: maxVel
    glm::vec4 startColor{0.0f, 0.8f, 1.0f, 1.0f};
    glm::vec4 endColor{0.8f, 0.1f, 0.9f, 0.0f};
    bool collisionFloor = false;
    float floorHeight = 0.0f;
    float restitution = 0.6f;
};

class ParticleBuffer {
public:
    ParticleBuffer(size_t capacity = 100000);
    ~ParticleBuffer() = default;

    void Resize(size_t capacity);
    size_t GetCapacity() const { return m_capacity; }
    size_t GetActiveCount() const { return m_activeCount; }

    void Reset();
    void StepSimulation(const ParticleSimParams& params);

    const std::vector<GpuParticle>& GetParticles() const { return m_particles; }
    std::vector<GpuParticle>& GetParticles() { return m_particles; }

    nf::GeometryData ToGeometryData() const;

private:
    size_t m_capacity = 100000;
    size_t m_activeCount = 0;
    std::vector<GpuParticle> m_particles;
    std::vector<GpuParticle> m_particlesBackBuffer;
};

} // namespace gpu
