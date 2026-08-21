# Implementation Plan — Phase 15 Epic 15.5: Real-Time Physics Engine & Rigid Body Dynamics Integration

**Phase:** 15 (Expansion Toward “Almost Everything”)  
**Epic:** 15.5 (Real-Time Physics Engine & Rigid Body Dynamics Integration — Bullet/PhysX SOP)  
**Status:** In Planning  
**Target Milestone:** M7 (Advanced Visual FX & Dynamic Tools)  
**Company:** Neo Realms  
**Owner:** Waleed Khalid  

---

## 1. Overview & Objectives

Epic 15.5 introduces a high-performance **Real-Time Physics & Rigid Body Dynamics Engine** to NodeForge. Built for interactive kinetic sculptures, procedural destruction arenas, falling domino stacks, ragdoll interactions, and collision-triggered audiovisual generative shows.

Key capabilities:
- **`PhysicsWorld` (`src/physics/PhysicsWorld.h/.cpp`)**: Native multi-body solver supporting deterministic fixed time-stepping, 6-DOF linear and angular momentum ($\vec{p} = m \vec{v}, \vec{L} = \mathbf{I} \vec{\omega}$), Continuous Collision Detection (CCD), impulse-based contact restitution and Coulomb friction, and spring/hinge constraints.
- **`PhysicsSolverComp` (Family: `Comp`)**: Master simulation controller managing global gravity $\vec{g} = (0, -9.81, 0)$, sub-stepping (1–8x), drag/damping, time scale, and global reset pulse. Outputs aggregated collision event channels.
- **`RigidBodyGeomOp` (Family: `GeomOp`)**: Converts 3D geometry into dynamic, static, or kinematic rigid bodies with mass $m$, restitution $e$, friction $\mu$, initial velocities, and impulse force injection. Emits transformed geometry meshes or high-count matrix instances.
- **`ColliderGeomOp` (Family: `GeomOp`)**: Assigns collision geometries (Box bounding, Sphere radius, Infinite Ground Plane, Convex Hull) and computes contact manifolds.
- **`PhysicsForceChanOp` (Family: `ChanOp`)**: Generates dynamic spatial physics forces (Point Attractors/Repulsors, Radial Explosions, Directional Wind, Vortex Swirls, and OSC/Gesture-driven impulse shockwaves).
- **Collision Feedback & Trigger Pipeline**: Real-time collision telemetry (`impact_force, contact_pos_xyz, collision_pulse`) enabling collisions to immediately trigger spatial audio sound effects (`AudioSpatializerChanOp`), particle bursts (`ParticleEmitterGeomOp`), and DMX strobe lights.

---

## 2. Architecture & Physics Mathematical Model (ADR-0018)

### 2.1 Rigid Body State Model
Every rigid body stores physical state:

```cpp
struct RigidBodyState {
    uint32_t id = 0;
    glm::vec3 position{0.0f};
    glm::quat orientation{1.0f, 0.0f, 0.0f, 0.0f};
    glm::vec3 linearVelocity{0.0f};
    glm::vec3 angularVelocity{0.0f};
    glm::vec3 forceAccum{0.0f};
    glm::vec3 torqueAccum{0.0f};
    float mass = 1.0f;
    float invMass = 1.0f;
    glm::mat3 inertiaTensor{1.0f};
    glm::mat3 invInertiaTensor{1.0f};
    float restitution = 0.5f;
    float friction = 0.3f;
    bool isStatic = false;
    bool isSleeping = false;
};
```

### 2.2 Numerical Integration (Semi-Implicit Euler)
For time step $\Delta t$:

$$\begin{aligned}
\vec{v}(t + \Delta t) &= \vec{v}(t) + \frac{\vec{F}_{\text{total}}}{m} \cdot \Delta t \\
\vec{\omega}(t + \Delta t) &= \vec{\omega}(t) + \mathbf{I}^{-1} \cdot \vec{\tau}_{\text{total}} \cdot \Delta t \\
\vec{x}(t + \Delta t) &= \vec{x}(t) + \vec{v}(t + \Delta t) \cdot \Delta t \\
\mathbf{q}(t + \Delta t) &= \text{normalize}\left( \mathbf{q}(t) + \frac{1}{2} [0, \vec{\omega}(t + \Delta t)] \otimes \mathbf{q}(t) \cdot \Delta t \right)
\end{aligned}$$

### 2.3 Impulse Contact Resolution
For contact normal $\hat{n}$ with relative approach velocity $v_{\text{rel}} = (\vec{v}_A - \vec{v}_B) \cdot \hat{n}$:

$$J = \frac{-(1 + e) \cdot v_{\text{rel}}}{\frac{1}{m_A} + \frac{1}{m_B} + [(\mathbf{I}_A^{-1}(\vec{r}_A \times \hat{n})) \times \vec{r}_A + (\mathbf{I}_B^{-1}(\vec{r}_B \times \hat{n})) \times \vec{r}_B] \cdot \hat{n}}$$

---

## 3. Operator Suite & Specifications

### 1. `PhysicsSolverComp` (Family: `Comp`)
- **Input Pins:** `in_bodies` (Comp, multi-input for rigid bodies & colliders), `in_forces` (Chan, external force channels).
- **Output Pins:** `output` (Comp), `out_events` (Chan, collision telemetry: `num_collisions, impact_force, contact_x, contact_y, contact_z`).
- **Parameters:**
  - `gravity`: `vec3` (default: `[0.0, -9.81, 0.0]`).
  - `sub_steps`: `int` (default: `4`, range: `1 .. 16`).
  - `linear_damping`: `float` (default: `0.02`).
  - `angular_damping`: `float` (default: `0.05`).
  - `time_scale`: `float` (default: `1.0`).
  - `reset`: `bool` reset simulation trigger.

### 2. `RigidBodyGeomOp` (Family: `GeomOp`)
- **Input Pins:** `template_geom` (Geom), `in_force` (Chan, dynamic impulse).
- **Output Pins:** `output` (Geom, transformed mesh or instanced geometry).
- **Parameters:**
  - `body_type`: `0: Dynamic`, `1: Static`, `2: Kinematic`.
  - `mass`: `float` (default: `1.0`).
  - `restitution`: `float` (default: `0.6`).
  - `friction`: `float` (default: `0.3`).
  - `initial_pos`: `vec3` (default: `[0.0, 5.0, 0.0]`).
  - `initial_vel`: `vec3` (default: `[0.0, 0.0, 0.0]`).
  - `collision_shape`: `0: Box`, `1: Sphere`, `2: Plane`, `3: Mesh Convex`.

### 3. `ColliderGeomOp` (Family: `GeomOp`)
- **Input Pins:** `in_geom` (Geom).
- **Output Pins:** `output` (Geom).
- **Parameters:**
  - `shape_type`: `0: Ground Plane`, `1: Box Bounds`, `2: Sphere Bounds`.
  - `size`: `vec3` collider dimensions (default: `[10.0, 1.0, 10.0]`).
  - `restitution`: `float` (default: `0.5`).
  - `friction`: `float` (default: `0.5`).

### 4. `PhysicsForceChanOp` (Family: `ChanOp`)
- **Input Pins:** `in_trigger` (Chan, optional trigger pulse).
- **Output Pins:** `out_force` (Chan, force vector stream `fx, fy, fz, magnitude`).
- **Parameters:**
  - `force_type`: `0: Directional Constant`, `1: Radial Explosion`, `2: Point Attractor`, `3: Vortex Swirl`.
  - `strength`: `float` (default: `100.0`).
  - `origin`: `vec3` (default: `[0.0, 0.0, 0.0]`).
  - `radius`: `float` (default: `15.0`).

---

## 4. Planned File Structure

```
d:\[Project]\Touch Designer\
├── docs\
│   ├── adr\
│   │   └── ADR-0018-real-time-physics-engine.md
│   ├── operator-spec\
│   │   ├── PhysicsSolverComp.md
│   │   ├── RigidBodyGeomOp.md
│   │   ├── ColliderGeomOp.md
│   │   └── PhysicsForceChanOp.md
│   └── cheat-sheets\
│       └── 11_physics_cheat_sheet.md
├── src\
│   ├── physics\
│   │   ├── PhysicsWorld.h/.cpp        # 6-DOF multi-body solver & collision detection
│   │   └── CollisionPrimitives.h/.cpp # SAT box, sphere, plane contact manifolds
│   └── operators\
│       ├── comp\
│       │   └── PhysicsSolverComp.h/.cpp
│       ├── geom\
│       │   ├── RigidBodyGeomOp.h/.cpp
│       │   └── ColliderGeomOp.h/.cpp
│       └── chan\
│           └── PhysicsForceChanOp.h/.cpp
├── samples\
│   └── 09_kinetic_physics_arena\
│       ├── kinetic_physics_arena.nfp  # Flagship interactive destruction arena
│       └── README.md
└── tests\
    ├── unit\
    │   └── physics_system_test.cpp    # Unit tests for SAT, momentum, restitution, forces
    └── benchmark\
        └── physics_benchmark.cpp      # 1,000-body collision throughput benchmark
```

---

## 5. Detailed Task Breakdown

### Task 15.5.1: Architecture Documentation & Operator Specifications
- **Agent:** `project-planner` / `architecture`
- **Priority:** P0 (Design Pre-requisite)
- **Output:**
  1. `docs/adr/ADR-0018-real-time-physics-engine.md`
  2. `docs/operator-spec/PhysicsSolverComp.md`
  3. `docs/operator-spec/RigidBodyGeomOp.md`
  4. `docs/operator-spec/ColliderGeomOp.md`
  5. `docs/operator-spec/PhysicsForceChanOp.md`
  6. `docs/cheat-sheets/11_physics_cheat_sheet.md`

### Task 15.5.2: Physics Simulation Engine & Operator Implementations
- **Agent:** `backend-specialist` / `game-developer`
- **Priority:** P0 (Core Foundation)
- **Output:**
  1. `src/physics/PhysicsWorld.h/.cpp`
  2. `src/physics/CollisionPrimitives.h/.cpp`
  3. `src/operators/comp/PhysicsSolverComp.h/.cpp`
  4. `src/operators/geom/RigidBodyGeomOp.h/.cpp`
  5. `src/operators/geom/ColliderGeomOp.h/.cpp`
  6. `src/operators/chan/PhysicsForceChanOp.h/.cpp`
  7. Register all operators in `src/graph/CoreNodes.cpp` and `CMakeLists.txt`.

### Task 15.5.3: Showcase Sample Project (`samples/09_kinetic_physics_arena/`)
- **Agent:** `project-planner` / `backend-specialist`
- **Priority:** P1 (Production Sample)
- **Output:**
  1. `samples/09_kinetic_physics_arena/kinetic_physics_arena.nfp`
  2. `samples/09_kinetic_physics_arena/README.md`

### Task 15.5.4: Automated Unit Tests & 1,000-Body Benchmark
- **Agent:** `test-engineer` / `backend-specialist`
- **Priority:** P0 (CI Quality Gate)
- **Output:**
  1. `tests/unit/physics_system_test.cpp`
  2. `tests/benchmark/physics_benchmark.cpp`
  3. Update `tests/CMakeLists.txt`.

---

## 6. Phase X: Final Verification Checklist

- [ ] `docs/adr/ADR-0018-real-time-physics-engine.md` written and approved
- [ ] Operator specs for all 4 physics operators created in `docs/operator-spec/`
- [ ] `PhysicsWorld`, `CollisionPrimitives`, `PhysicsSolverComp`, `RigidBodyGeomOp`, `ColliderGeomOp`, `PhysicsForceChanOp` implemented
- [ ] Operators registered in `CoreNodes.cpp` and `CMakeLists.txt`
- [ ] `samples/09_kinetic_physics_arena/kinetic_physics_arena.nfp` created with documentation
- [ ] `tests/unit/physics_system_test.cpp` and `tests/benchmark/physics_benchmark.cpp` added to CMake
- [ ] 1,000-body collision benchmark verified with sub-5.0 ms cook times and zero memory leaks
- [ ] All automated tests passing (100% pass rate)
- [ ] `STATUS.md` and `.agent/memory/MEMORY.md` updated
