# Operator Specification: ParticleEmitterGeomOp

**Family:** `GeomOp`  
**Type Name:** `ParticleEmitterGeomOp`  
**Version:** 1.0  
**Phase Ownership:** Phase 15 (Epic 15.1)  

---

## 1. Description

`ParticleEmitterGeomOp` is the root generator node of the GPU particle simulation subsystem. It spawns continuous streams of particles from a point origin, bounding volume (sphere, box), or arbitrary 3D mesh surface connected to its optional `emitter_mesh` pin.

---

## 2. Pins

### Input Pins
| Pin Name | Type | Required | Description |
|---|---|---|---|
| `emitter_mesh` | `Geom` | No | Optional 3D surface mesh from which particles are spawned across surface triangles |
| `spawn_trigger` | `Chan` | No | Optional channel pulse/rate trigger modulating birth bursts |

### Output Pins
| Pin Name | Type | Description |
|---|---|---|
| `output` | `Geom` | Stream of active particle geometry vertices |

---

## 3. Parameters

| Parameter | Type | Default | Range | Description |
|---|---|---|---|---|
| `max_particles` | `int` | `100000` | `1000 .. 1000000` | Total allocated particle buffer capacity |
| `birth_rate` | `float` | `5000.0` | `0.0 .. 500000.0` | Continuous particles spawned per second |
| `lifetime` | `float` | `3.0` | `0.1 .. 60.0` | Base particle lifespan in seconds |
| `lifetime_var`| `float` | `0.5` | `0.0 .. 1.0` | Random variance percentage applied to lifetime |
| `initial_speed`| `float` | `2.0` | `0.0 .. 100.0` | Initial velocity magnitude |
| `spread_angle`| `float` | `25.0` | `0.0 .. 180.0` | Directional dispersion cone angle in degrees |
| `emitter_type`| `int` | `0` | `0 .. 2` | 0: Point, 1: Sphere Volume, 2: Box Volume |
| `emitter_size`| `vec3` | `[1.0, 1.0, 1.0]` | `[0.01 .. 100.0]` | Size dimensions of the spawn volume |
| `start_color` | `vec4` | `[0.0, 0.8, 1.0, 1.0]` | `[0.0 .. 1.0]` | Particle birth color (RGBA) |
| `end_color` | `vec4` | `[0.8, 0.1, 0.9, 0.0]` | `[0.0 .. 1.0]` | Particle death color (RGBA) |
| `initial_size`| `float` | `0.05` | `0.001 .. 5.0` | Base particle radius |
| `reset` | `pulse`| `0` | `0 .. 1` | Clears all active particles and resets simulation |

---

## 4. Cook Semantics

- **Time-Dependent:** Yes. Cooks every active timeline frame during playback.
- **Resource Management:** Leases memory from `ParticleBuffer`.
