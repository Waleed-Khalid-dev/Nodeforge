#include "ParticleEmitterGeomOp.h"

namespace nf {

ParticleEmitterGeomOp::ParticleEmitterGeomOp(NodeId id, const std::string& name)
    : GeomOp(id, name, "ParticleEmitterGeomOp") {
    m_inMeshPin = AddInputPin("emitter_mesh", PinType::Geom);
    m_inTriggerPin = AddInputPin("spawn_trigger", PinType::Chan);
    m_outPin = AddOutputPin("output", PinType::Geom);

    SetParam("max_particles", static_cast<int32_t>(100000));
    SetParam("birth_rate", 5000.0f);
    SetParam("lifetime", 3.0f);
    SetParam("lifetime_var", 0.5f);
    SetParam("initial_speed", 2.0f);
    SetParam("spread_angle", 25.0f);
    SetParam("emitter_type", static_cast<int32_t>(0)); // 0: Point, 1: Sphere, 2: Box
    SetParam("emitter_size", glm::vec3(1.0f));
    SetParam("start_color", glm::vec4(0.0f, 0.8f, 1.0f, 1.0f));
    SetParam("end_color", glm::vec4(0.8f, 0.1f, 0.9f, 0.0f));
    SetParam("initial_size", 0.05f);
    SetParam("reset", false);

    m_particleBuffer.Resize(100000);
}

bool ParticleEmitterGeomOp::Cook(const CookContext& context) {
    bool reset = GetParam("reset").Is<bool>() ? GetParam("reset").Get<bool>() : false;
    if (reset) {
        m_particleBuffer.Reset();
    }

    int32_t maxP = GetParam("max_particles").Is<int32_t>() ? GetParam("max_particles").Get<int32_t>() : 100000;
    if (static_cast<size_t>(maxP) != m_particleBuffer.GetCapacity()) {
        m_particleBuffer.Resize(static_cast<size_t>(std::max(100, maxP)));
    }

    gpu::ParticleSimParams params{};
    params.deltaTime = static_cast<float>(context.deltaTimeSeconds);
    params.time = static_cast<float>(context.timeSeconds);
    params.particleCount = static_cast<uint32_t>(maxP);
    params.birthRate = GetParam("birth_rate").Is<float>() ? GetParam("birth_rate").Get<float>() : 5000.0f;
    params.lifetime = GetParam("lifetime").Is<float>() ? GetParam("lifetime").Get<float>() : 3.0f;
    params.lifetimeVar = GetParam("lifetime_var").Is<float>() ? GetParam("lifetime_var").Get<float>() : 0.5f;
    params.initialSpeed = GetParam("initial_speed").Is<float>() ? GetParam("initial_speed").Get<float>() : 2.0f;
    params.spreadAngle = GetParam("spread_angle").Is<float>() ? GetParam("spread_angle").Get<float>() : 25.0f;
    params.startColor = GetParam("start_color").Is<glm::vec4>() ? GetParam("start_color").Get<glm::vec4>() : glm::vec4(0.0f, 0.8f, 1.0f, 1.0f);
    params.endColor = GetParam("end_color").Is<glm::vec4>() ? GetParam("end_color").Get<glm::vec4>() : glm::vec4(0.8f, 0.1f, 0.9f, 0.0f);

    m_particleBuffer.StepSimulation(params);
    SetOutputGeometry(m_particleBuffer.ToGeometryData());

    return true;
}

} // namespace nf
