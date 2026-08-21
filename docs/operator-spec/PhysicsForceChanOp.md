# Operator Specification: PhysicsForceChanOp

**Family:** `ChanOp`  
**Type Name:** `PhysicsForceChanOp`  
**Version:** 1.0  
**Phase Ownership:** Phase 15 (Epic 15.5)  

---

## 1. Description

`PhysicsForceChanOp` generates dynamic spatial force vector streams (Point Attractor/Repulsor, Radial Explosion Blast, Directional Wind, Vortex Swirls) for injecting physical impulses into rigid bodies.

---

## 2. Pins

### Input Pins
| Pin Name | Type | Required | Description |
|---|---|---|---|
| `in_trigger` | `Chan` | No | Optional dynamic impulse trigger pulse |

### Output Pins
| Pin Name | Type | Description |
|---|---|---|
| `out_force` | `Chan` | Generated force vector channel stream (`fx, fy, fz, magnitude`) |

---

## 3. Parameters

| Parameter | Type | Default | Range | Description |
|---|---|---|---|---|
| `force_type` | `int` | `0` | `0 .. 3` | 0: Directional Wind, 1: Radial Explosion, 2: Point Attractor, 3: Vortex Swirl |
| `strength` | `float` | `100.0` | `-10000.0 .. 10000.0` | Force amplitude |
| `direction` | `vec3` | `[0.0, 0.0, 1.0]` | - | Vector direction for directional wind |
| `origin` | `vec3` | `[0.0, 0.0, 0.0]` | - | Spatial center for radial/point/vortex forces |
| `radius` | `float` | `15.0` | `0.1 .. 1000.0` | Outer radius of force influence |

---

## 4. Cook Semantics

- **Time-Dependent:** Yes.
