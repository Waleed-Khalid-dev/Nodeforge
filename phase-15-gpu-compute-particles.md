# Implementation Plan — Phase 15 Epic 15.1: GPU Compute Particle Simulation System

**Phase:** 15 (Expansion Toward “Almost Everything”)  
**Epic:** 15.1 (GPU Compute Particle Engine & Interactive Swarms)  
**Status:** In Planning  
**Target Milestone:** M7 (Advanced Visual FX & Dynamic Simulations)  
**Company:** Neo Realms  
**Owner:** Waleed Khalid  

---

## 1. Overview & Objectives

Epic 15.1 introduces a native, real-time **GPU Compute Particle Simulation System** to NodeForge. Engineered from the ground up on **Vulkan 1.3 Compute Pipelines** with dynamic descriptor sets and double-buffered GPU storage buffers (`VkBuffer`), this system simulates over **1,000,000 active particles at 60+ FPS** on standard NVIDIA RTX GPUs.

Designed specifically to power Neo Realms' interactive holograms, architectural projection mapping visuals, and immersive gesture experiences, the particle system seamlessly integrates with all existing engine families:
- **GeomOps**: Emit particles directly from arbitrary 3D mesh surfaces (Grids, Spheres, Boxes, custom geometries).
- **ChanOps**: Modulate particle forces, turbulence fields, and point attractors in real-time using live OSC LiDAR gesture coordinates, audio frequencies, and MIDI CC faders.
- **TexOps**: Render particles via point-sprites or camera-facing billboard materials with soft depth fading and composite them through bloom filters, 2D Bezier warpers, and multi-projector outputs.

---

## 2. Architecture & Data Structures (ADR-0014)

### 2.1 GPU Particle Buffer Layout
Particles are stored in double-buffered GPU VMA allocations (`VkBuffer` with `VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT`):

```cpp
struct ParticleState {
    glm::vec4 positionAndLife; // xyz: position (world), w: normalized remaining lifetime [0.0 .. 1.0]
    glm::vec4 velocityAndMass; // xyz: velocity vector, w: inverse mass
    glm::vec4 colorAndSize;    // rgba: particle base color, w: radius/point size
};
```

### 2.2 Compute Pipeline & Modular Force Integration
The simulation step executes a dedicated Vulkan compute shader pass (`shaders/particles/particle_sim.comp`) using Euler / Verlet numerical integration:
1. **Emitter Stage:** Spawns dead/recycled particles from emitter mesh surfaces with initial velocity, lifetime, and spread angle.
2. **Force Accumulation Stage:**
   - Global gravity and ambient drag / air resistance.
   - 3D Simplex Curl Noise vector fields generating organic fluid-like turbulence without volume divergence.
   - Vortex / orbital angular momentum forces.
3. **Attractor / Interaction Stage:**
   - Point attractors and repulsors driven by OSC gesture coordinates (`/tracking/x`, `/tracking/y`).
   - Bounding volume collisions and planar bounce damping.
4. **Life & State Recycling:**
   - Continuous aging decay; particles with `lifetime <= 0.0` are recycled into an atomic dead ring-index queue.

---

## 3. Operator Suite & Specifications

### 1. `ParticleEmitterGeomOp` (Family: `GeomOp`)
- **Input Pins:** `emitter_mesh` (Geom, optional), `spawn_trigger` (Chan, optional).
- **Output Pins:** `output` (Geom / Particle Stream).
- **Parameters:**
  - `max_particles`: Total buffer capacity (default: `100000`, up to `1000000`).
  - `birth_rate`: Particles spawned per second (default: `5000.0`).
  - `lifetime`: Lifetime in seconds (default: `3.0`).
  - `lifetime_var`: Lifetime variance (default: `0.5`).
  - `initial_speed`: Base velocity magnitude (default: `2.0`).
  - `spread_angle`: Cone dispersion angle in degrees (default: `25.0`).
  - `start_color`: Base RGBA color (default: `[0.0, 0.8, 1.0, 1.0]`).
  - `end_color`: Death RGBA color (default: `[0.8, 0.1, 0.9, 0.0]`).
  - `initial_size`: Particle radius in world units (default: `0.05`).

### 2. `ParticleForceGeomOp` (Family: `GeomOp`)
- **Input Pins:** `input` (Geom / Particle Stream), `force_mod` (Chan, optional).
- **Output Pins:** `output` (Geom / Particle Stream).
- **Parameters:**
  - `gravity`: World gravity vector (default: `[0.0, -1.0, 0.0]`).
  - `drag`: Viscous air resistance coefficient (default: `0.15`).
  - `turbulence_strength`: 3D Curl noise amplitude (default: `1.5`).
  - `turbulence_frequency`: Spatial scale of curl noise (default: `0.5`).
  - `turbulence_speed`: Temporal animation rate (default: `0.3`).
  - `vortex_axis`: Rotation axis vector (default: `[0.0, 1.0, 0.0]`).
  - `vortex_strength`: Swirl angular force (default: `0.0`).

### 3. `ParticleAttractorGeomOp` (Family: `GeomOp`)
- **Input Pins:** `input` (Geom / Particle Stream), `target_pos` (Chan / Vec3).
- **Output Pins:** `output` (Geom / Particle Stream).
- **Parameters:**
  - `attractor_type`: Attract (0) or Repel (1).
  - `position`: World 3D coordinate (default: `[0.0, 0.0, 0.0]`, dynamic via Python/ChanOp).
  - `strength`: Attraction force magnitude (default: `5.0`).
  - `radius`: Sphere of influence radius (default: `3.0`).
  - `falloff`: Linear (0), Inverse Square (1), Smoothstep (2).

### 4. `ParticleMatOp` (Family: `MatOp`)
- **Input Pins:** None.
- **Output Pins:** `output` (Mat).
- **Parameters:**
  - `blend_mode`: Additive (0), Alpha Blend (1), Screen (2).
  - `point_type`: Point Sprite (0), Camera-Facing Quad Billboard (1), Motion Streak (2).
  - `texture_map`: Leased 2D texture (TexOp, e.g. soft circle or flare sprite).
  - `depth_fade`: Distance in meters to soften intersection with opaque 3D scene meshes (default: `0.2`).
  - `size_attenuation`: Scales particle screen size inversely with camera distance.

---

## 4. Planned File Structure

```
d:\[Project]\Touch Designer\
├── docs\
│   ├── adr\
│   │   └── ADR-0014-gpu-compute-particles.md
│   ├── operator-spec\
│   │   ├── ParticleEmitterGeomOp.md
│   │   ├── ParticleForceGeomOp.md
│   │   ├── ParticleAttractorGeomOp.md
│   │   └── ParticleMatOp.md
│   └── cheat-sheets\
│       └── 07_particles_cheat_sheet.md
├── shaders\
│   └── particles\
│       ├── particle_sim.comp           # Vulkan 1.3 GLSL Compute Shader
│       ├── particle_point.vert         # Point sprite / Billboard vertex shader
│       └── particle_point.frag         # Soft circular / Textured fragment shader
├── src\
│   ├── gpu\
│   │   ├── ParticleBuffer.h/.cpp       # Double-buffered GPU storage allocations
│   │   └── ParticleComputePass.h/.cpp  # Vulkan compute pipeline dispatcher
│   └── operators\
│       ├── geom\
│       │   ├── ParticleEmitterGeomOp.h/.cpp
│       │   ├── ParticleForceGeomOp.h/.cpp
│       │   └── ParticleAttractorGeomOp.h/.cpp
│       └── mat\
│           └── ParticleMatOp.h/.cpp
├── samples\
│   └── 05_holographic_particles\
│       ├── holographic_particles.nfp   # Flagship interactive swarm & gesture show
│       └── README.md
└── tests\
    ├── unit\
    │   └── particle_system_test.cpp    # Unit tests for compute, buffer & life-cycle
    └── benchmark\
        └── particle_benchmark.cpp      # 1M particle 60 FPS continuous stress test
```

---

## 5. Detailed Task Breakdown

### Task 15.1.1: Architecture Documentation & Operator Specifications
- **Agent:** `project-planner` / `architecture`
- **Priority:** P0 (Design Pre-requisite)
- **Input:** Clean-room Vulkan 1.3 compute design principles.
- **Output:**
  1. `docs/adr/ADR-0014-gpu-compute-particles.md`
  2. `docs/operator-spec/ParticleEmitterGeomOp.md`
  3. `docs/operator-spec/ParticleForceGeomOp.md`
  4. `docs/operator-spec/ParticleAttractorGeomOp.md`
  5. `docs/operator-spec/ParticleMatOp.md`
  6. `docs/cheat-sheets/07_particles_cheat_sheet.md`
- **Verify:** Complete specs with pin signatures, parameters, cook semantics, and memory budgets.

### Task 15.1.2: Vulkan Compute Shaders & GPU Particle Subsystem
- **Agent:** `backend-specialist`
- **Priority:** P0 (Core Foundation)
- **Input:** Vulkan 1.3 `Device`, `VMA`, `ShaderCompiler`.
- **Output:**
  1. `shaders/particles/particle_sim.comp`, `particle_point.vert`, `particle_point.frag`
  2. `src/gpu/ParticleBuffer.h/.cpp` (double-buffered VMA GPU storage buffers)
  3. `src/gpu/ParticleComputePass.h/.cpp` (Vulkan compute pipeline creation, descriptor layouts, and command buffer dispatch)
- **Verify:** Headless compute shader compilation to SPIR-V and memory buffer allocation tests.

### Task 15.1.3: Particle Operator Implementations
- **Agent:** `backend-specialist`
- **Priority:** P0 (Operators)
- **Input:** `ParticleBuffer`, `GeomOp` base class, `MatOp` base class, `NodeRegistry`.
- **Output:**
  1. `src/operators/geom/ParticleEmitterGeomOp.h/.cpp`
  2. `src/operators/geom/ParticleForceGeomOp.h/.cpp`
  3. `src/operators/geom/ParticleAttractorGeomOp.h/.cpp`
  4. `src/operators/mat/ParticleMatOp.h/.cpp`
  5. Register all 4 new operators in `src/graph/CoreNodes.cpp`.
- **Verify:** Graph instantiation, parameter evaluation, and DAG connection tests.

### Task 15.1.4: Showcase Sample Project (`samples/05_holographic_particles/`)
- **Agent:** `project-planner` / `backend-specialist`
- **Priority:** P1 (Production Sample)
- **Input:** `.nfp` JSON v1 serializer, `OSCInChanOp`, `AudioFileInChanOp`, `WarpBlendTexOp`.
- **Output:**
  1. `samples/05_holographic_particles/holographic_particles.nfp` (Interactive gesture attractor + audio-reactive curl noise storm)
  2. `samples/05_holographic_particles/README.md`
- **Verify:** Project loads seamlessly in `ProjectSerializer` and standalone `nodeforge_player.exe`.

### Task 15.1.5: Automated Unit Tests & 1M Particle Stress Benchmark
- **Agent:** `test-engineer` / `backend-specialist`
- **Priority:** P0 (CI Gate)
- **Input:** GoogleTest, Google Benchmark, `CookProfiler`.
- **Output:**
  1. `tests/unit/particle_system_test.cpp` (Compute dispatch, lifecycle recycling, attractor math, JSON serialization)
  2. `tests/benchmark/particle_benchmark.cpp` (100k, 500k, and 1,000,000 particle throughput benchmarks)
  3. Register targets in `tests/CMakeLists.txt`.
- **Verify:** `ctest` passes 100% with 0 memory leaks across 10,000 continuous frames.

---

## 6. Phase X: Final Verification Checklist

- [ ] `docs/adr/ADR-0014-gpu-compute-particles.md` written and approved
- [ ] Operator specs for all 4 particle operators created in `docs/operator-spec/`
- [ ] GLSL compute and point-sprite shaders written in `shaders/particles/`
- [ ] `ParticleBuffer` and `ParticleComputePass` implemented in `src/gpu/`
- [ ] 4 particle operators implemented and registered in `CoreNodes.cpp`
- [ ] `samples/05_holographic_particles/holographic_particles.nfp` created with documentation
- [ ] `tests/unit/particle_system_test.cpp` and `tests/benchmark/particle_benchmark.cpp` added to CMake
- [ ] 1,000,000 particles benchmarked at 60+ FPS with 0 memory leaks over 10,000 frames
- [ ] All automated tests passing (100% pass rate)
- [ ] `STATUS.md` and `.agent/memory/MEMORY.md` updated
