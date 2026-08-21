# Operator Specification: ColliderGeomOp

**Family:** `GeomOp`  
**Type Name:** `ColliderGeomOp`  
**Version:** 1.0  
**Phase Ownership:** Phase 15 (Epic 15.5)  

---

## 1. Description

`ColliderGeomOp` defines static collision geometry obstacles (Infinite Ground Plane, Box Bounds, Sphere Bounds) for physical rigid bodies to bounce off and slide across.

---

## 2. Pins

### Input Pins
| Pin Name | Type | Required | Description |
|---|---|---|---|
| `in_geom` | `Geom` | No | Optional mesh providing bounding shape |

### Output Pins
| Pin Name | Type | Description |
|---|---|---|
| `output` | `Geom` | Static collider geometry descriptor |

---

## 3. Parameters

| Parameter | Type | Default | Range | Description |
|---|---|---|---|---|
| `shape_type` | `int` | `0` | `0 .. 2` | 0: Ground Plane, 1: Box Bounds, 2: Sphere Bounds |
| `size` | `vec3` | `[20.0, 1.0, 20.0]` | - | Collider dimensions |
| `position` | `vec3` | `[0.0, 0.0, 0.0]` | - | Collider position |
| `restitution` | `float` | `0.5` | `0.0 .. 1.0` | Bounce elasticity |
| `friction` | `float` | `0.5` | `0.0 .. 1.0` | Surface friction |

---

## 4. Cook Semantics

- **Time-Dependent:** No.
