# Operator Specification: RigidBodyGeomOp

**Family:** `GeomOp`  
**Type Name:** `RigidBodyGeomOp`  
**Version:** 1.0  
**Phase Ownership:** Phase 15 (Epic 15.5)  

---

## 1. Description

`RigidBodyGeomOp` converts input 3D geometry into physical 6-DOF dynamic, static, or kinematic rigid bodies. It simulates real-time translations and rotations and outputs transformed meshes or matrix instances.

---

## 2. Pins

### Input Pins
| Pin Name | Type | Required | Description |
|---|---|---|---|
| `template_geom` | `Geom` | Yes | 3D mesh geometry to physically simulate |
| `in_force` | `Chan` | No | Optional dynamic impulse force channel |

### Output Pins
| Pin Name | Type | Description |
|---|---|---|
| `output` | `Geom` | Physically simulated transformed 3D geometry mesh |

---

## 3. Parameters

| Parameter | Type | Default | Range | Description |
|---|---|---|---|---|
| `body_type` | `int` | `0` | `0 .. 2` | 0: Dynamic, 1: Static, 2: Kinematic |
| `mass` | `float` | `1.0` | `0.001 .. 10000.0`| Rigid body mass in kg |
| `restitution` | `float` | `0.6` | `0.0 .. 1.0` | Elastic bounciness coefficient |
| `friction` | `float` | `0.3` | `0.0 .. 1.0` | Coulomb surface friction coefficient |
| `initial_pos` | `vec3` | `[0.0, 5.0, 0.0]` | - | Starting spawn position |
| `initial_vel` | `vec3` | `[0.0, 0.0, 0.0]` | - | Initial linear velocity |
| `collision_shape` | `int` | `0` | `0 .. 3` | 0: Box, 1: Sphere, 2: Plane, 3: Convex |

---

## 4. Cook Semantics

- **Time-Dependent:** Yes.
