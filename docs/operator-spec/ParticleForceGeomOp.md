# Operator Specification: ParticleForceGeomOp

**Family:** `GeomOp`  
**Type Name:** `ParticleForceGeomOp`  
**Version:** 1.0  
**Phase Ownership:** Phase 15 (Epic 15.1)  

---

## 1. Description

`ParticleForceGeomOp` applies physical dynamic forces to an incoming particle stream, including directional gravity, air drag, 3D Simplex Curl turbulence noise fields, and vortex swirl rotations.

---

## 2. Pins

### Input Pins
| Pin Name | Type | Required | Description |
|---|---|---|---|
| `input` | `Geom` | Yes | Incoming particle stream |
| `force_mod` | `Chan` | No | Optional numeric channels modulating force strength |

### Output Pins
| Pin Name | Type | Description |
|---|---|---|
| `output` | `Geom` | Updated particle stream with accumulated forces |

---

## 3. Parameters

| Parameter | Type | Default | Range | Description |
|---|---|---|---|---|
| `gravity` | `vec3` | `[0.0, -1.0, 0.0]` | `[-100.0 .. 100.0]` | Global gravitational acceleration vector |
| `drag` | `float` | `0.15` | `0.0 .. 10.0` | Viscous fluid/air resistance coefficient |
| `turbulence_strength` | `float` | `1.5` | `0.0 .. 50.0` | 3D Simplex Curl noise force amplitude |
| `turbulence_frequency`| `float` | `0.5` | `0.01 .. 20.0` | Spatial frequency scale of curl noise |
| `turbulence_speed` | `float` | `0.3` | `0.0 .. 10.0` | Temporal evolution speed of noise field |
| `vortex_axis` | `vec3` | `[0.0, 1.0, 0.0]` | `[-1.0 .. 1.0]` | Axis vector of rotational vortex force |
| `vortex_strength` | `float` | `0.0` | `-50.0 .. 50.0` | Angular rotational swirl acceleration |
| `collision_floor` | `bool` | `true` | `true/false` | Enables ground plane collision at Y = `floor_height` |
| `floor_height` | `float` | `0.0` | `[-100.0 .. 100.0]` | Y-plane elevation for ground bounce |
| `restitution` | `float` | `0.6` | `0.0 .. 1.0` | Bounciness / elasticity of surface collisions |

---

## 4. Cook Semantics

- **Time-Dependent:** Yes. Evaluates force vectors against particle velocity and positions each frame.
