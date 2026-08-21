# Operator Specification: PhysicsSolverComp

**Family:** `Comp`  
**Type Name:** `PhysicsSolverComp`  
**Version:** 1.0  
**Phase Ownership:** Phase 15 (Epic 15.5)  

---

## 1. Description

`PhysicsSolverComp` is the master simulation controller managing global gravity, time sub-stepping, linear/angular damping, and collision event aggregation.

---

## 2. Pins

### Input Pins
| Pin Name | Type | Required | Description |
|---|---|---|---|
| `in_bodies` | `Comp` | No | Rigid bodies and colliders registered into the physics world |
| `in_forces` | `Chan` | No | External dynamic force channel vectors |

### Output Pins
| Pin Name | Type | Description |
|---|---|---|
| `output` | `Comp` | Master physics world component descriptor |
| `out_events` | `Chan` | Real-time collision telemetry (`num_collisions, impact_force, contact_x, contact_y, contact_z`) |

---

## 3. Parameters

| Parameter | Type | Default | Range | Description |
|---|---|---|---|---|
| `gravity` | `vec3` | `[0.0, -9.81, 0.0]` | - | Global gravity acceleration vector |
| `sub_steps` | `int` | `4` | `1 .. 16` | Sub-step integration iterations per frame |
| `linear_damping` | `float` | `0.02` | `0.0 .. 1.0` | Atmospheric linear velocity drag |
| `angular_damping`| `float` | `0.05` | `0.0 .. 1.0` | Rotational velocity drag |
| `time_scale` | `float` | `1.0` | `0.0 .. 10.0` | Simulation playback time multiplier |
| `reset` | `bool` | `false` | `true/false` | Pulse parameter resetting simulation to initial states |

---

## 4. Cook Semantics

- **Time-Dependent:** Yes (advances multi-body numerical integration on each frame).
