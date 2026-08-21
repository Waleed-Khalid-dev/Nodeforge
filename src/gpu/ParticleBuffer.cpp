#include "ParticleBuffer.h"
#include <cmath>
#include <random>
#include <algorithm>

namespace gpu {

static float Hash11(uint32_t n) {
    n = (n << 13U) ^ n;
    n = n * (n * n * 15731U + 789221U) + 1376312589U;
    return static_cast<float>(n & 0x7fffffffU) / static_cast<float>(0x7fffffff);
}

static glm::vec3 Hash31(uint32_t n) {
    return glm::vec3(Hash11(n), Hash11(n + 19849U), Hash11(n + 65537U));
}

ParticleBuffer::ParticleBuffer(size_t capacity)
    : m_capacity(capacity) {
    m_particles.resize(m_capacity);
    m_particlesBackBuffer.resize(m_capacity);
    Reset();
}

void ParticleBuffer::Resize(size_t capacity) {
    m_capacity = capacity;
    m_particles.resize(m_capacity);
    m_particlesBackBuffer.resize(m_capacity);
    Reset();
}

void ParticleBuffer::Reset() {
    m_activeCount = 0;
    for (size_t i = 0; i < m_capacity; ++i) {
        m_particles[i].posLife = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
        m_particles[i].velMass = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        m_particles[i].colorSize = glm::vec4(1.0f, 1.0f, 1.0f, 0.05f);
    }
}

void ParticleBuffer::StepSimulation(const ParticleSimParams& params) {
    float dt = std::max(0.0001f, params.deltaTime);
    size_t count = std::min(m_capacity, static_cast<size_t>(params.particleCount));
    m_activeCount = 0;

    // Number of new particles to spawn this frame
    size_t spawnBudget = static_cast<size_t>(params.birthRate * dt);

    for (size_t i = 0; i < count; ++i) {
        GpuParticle p = m_particles[i];
        float life = p.posLife.w;

        // Age particle
        life -= dt / std::max(0.001f, params.lifetime);

        if (life <= 0.0f) {
            if (spawnBudget > 0) {
                --spawnBudget;
                uint32_t seed = static_cast<uint32_t>(i + static_cast<size_t>(params.time * 1000.0f));
                glm::vec3 rnd = Hash31(seed) * 2.0f - 1.0f;
                if (glm::length(rnd) > 1e-4f) {
                    rnd = glm::normalize(rnd);
                }

                p.posLife = glm::vec4(rnd * 0.2f, 1.0f - (Hash11(seed + 77U) * params.lifetimeVar * 0.5f));
                float speed = params.initialSpeed * (0.8f + 0.4f * Hash11(seed + 123U));
                p.velMass = glm::vec4(rnd * speed, 1.0f);
                p.colorSize = glm::vec4(glm::vec3(params.startColor), 0.05f);
                m_activeCount++;
            } else {
                p.posLife.w = 0.0f; // Remains dead
            }
        } else {
            glm::vec3 pos = glm::vec3(p.posLife);
            glm::vec3 vel = glm::vec3(p.velMass);

            // 1. Gravity & Drag
            glm::vec3 accel = glm::vec3(params.gravityAndDrag);

            // 2. 3D Curl Turbulence approximation
            if (params.turbulenceParams.x > 0.0f) {
                float freq = params.turbulenceParams.y;
                float spd = params.turbulenceParams.z;
                glm::vec3 coord = pos * freq + glm::vec3(0.0f, 0.0f, params.time * spd);
                float curlX = std::sin(coord.y * 3.14f) * std::cos(coord.z * 3.14f);
                float curlY = std::sin(coord.z * 3.14f) * std::cos(coord.x * 3.14f);
                float curlZ = std::sin(coord.x * 3.14f) * std::cos(coord.y * 3.14f);
                accel += glm::vec3(curlX, curlY, curlZ) * params.turbulenceParams.x * 5.0f;
            }

            // 3. Vortex Swirl
            if (std::abs(params.turbulenceParams.w) > 1e-4f) {
                glm::vec3 axis(0.0f, 1.0f, 0.0f);
                glm::vec3 rad = pos - axis * glm::dot(pos, axis);
                glm::vec3 tanDir = glm::cross(axis, rad);
                if (glm::length(tanDir) > 1e-4f) {
                    accel += glm::normalize(tanDir) * params.turbulenceParams.w;
                }
            }

            // 4. Point Attractor / Repulsor
            if (std::abs(params.attractorParams.w) > 1e-4f) {
                glm::vec3 toTarget = glm::vec3(params.attractorParams) - pos;
                float dist = glm::length(toTarget);
                float radius = params.attractorSettings.x;
                if (dist < radius && dist > 0.01f) {
                    glm::vec3 dir = toTarget / dist;
                    float falloff = 1.0f - (dist / radius);
                    if (params.attractorSettings.z == 1.0f) {
                        falloff = 1.0f / (dist * dist + 0.1f);
                    }
                    float sign = (params.attractorSettings.y == 0.0f) ? 1.0f : -1.0f;
                    accel += dir * (params.attractorParams.w * falloff * sign * 10.0f);
                }
            }

            // Integrate Velocity
            vel += accel * dt;
            vel *= std::max(0.0f, 1.0f - params.gravityAndDrag.w * dt);

            // Clamp max speed
            float maxSpd = params.attractorSettings.w;
            if (glm::length(vel) > maxSpd) {
                vel = glm::normalize(vel) * maxSpd;
            }

            // Integrate Position
            pos += vel * dt;

            // Collision with floor
            if (params.collisionFloor && pos.y < params.floorHeight) {
                pos.y = params.floorHeight;
                vel.y = -vel.y * params.restitution;
            }

            p.posLife = glm::vec4(pos, life);
            p.velMass = glm::vec4(vel, p.velMass.w);

            // Color progression over life
            float progress = 1.0f - life;
            p.colorSize = glm::vec4(glm::mix(glm::vec3(params.startColor), glm::vec3(params.endColor), progress), p.colorSize.w);
            m_activeCount++;
        }

        m_particlesBackBuffer[i] = p;
    }

    m_particles = m_particlesBackBuffer;
}

nf::GeometryData ParticleBuffer::ToGeometryData() const {
    std::vector<nf::Vertex> vertices;
    vertices.reserve(m_activeCount);
    std::vector<uint32_t> indices;
    indices.reserve(m_activeCount);

    uint32_t idx = 0;
    for (size_t i = 0; i < m_capacity; ++i) {
        const auto& p = m_particles[i];
        if (p.posLife.w > 0.0f) {
            nf::Vertex v{};
            v.pos = glm::vec3(p.posLife);
            v.normal = glm::vec3(0.0f, 1.0f, 0.0f);
            v.uv = glm::vec2(0.5f, 0.5f);
            v.color = glm::vec4(glm::vec3(p.colorSize), p.posLife.w);
            vertices.push_back(v);
            indices.push_back(idx++);
        }
    }

    return nf::GeometryData(std::move(vertices), std::move(indices));
}

} // namespace gpu
