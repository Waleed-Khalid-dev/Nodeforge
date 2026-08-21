# Particle Subsystem Cheat Sheet (GPU Compute Particles)

**Subsystem:** GPU Compute Particles | **Version:** 1.0 (Phase 15 Epic 15.1) | **Families:** `GeomOp`, `MatOp`

---

## 1. Particle Node Family Reference

| Operator | Family | In Pins | Out Pins | Key Roles |
|---|---|---|---|---|
| **`ParticleEmitterGeomOp`** | `GeomOp` | `emitter_mesh` (Geom), `spawn_trigger` (Chan) | `output` (Geom) | Spawns particles from volume/mesh, controls birth rate, lifetime, speed, and color |
| **`ParticleForceGeomOp`** | `GeomOp` | `input` (Geom), `force_mod` (Chan) | `output` (Geom) | Applies gravity, drag, 3D curl noise turbulence, and vortex swirl |
| **`ParticleAttractorGeomOp`**| `GeomOp` | `input` (Geom), `target_pos` (Chan) | `output` (Geom) | Dynamic 3D point attractors/repulsors driven by OSC/gesture inputs |
| **`ParticleMatOp`** | `MatOp` | `texture` (Tex) | `output` (Mat) | Billboard/point-sprite shader with additive blending and depth softening |

---

## 2. Standard Network Wiring Idioms

```
# Interactive Hand-Tracking Holographic Swarm
ParticleEmitter (Sphere Volume)
       │
       ▼
ParticleForce (Curl Turbulence: 2.0, Drag: 0.2)
       │
       ▼
ParticleAttractor (Position: [op('osc')['x'], op('osc')['y'], 0.0])
       │
       ├─────────────────────────────────┐
       ▼                                 ▼
GeometryComp ◄── [ParticleMatOp]      RenderTexOp ──► BlurTexOp ──► ProjectorOut
```
