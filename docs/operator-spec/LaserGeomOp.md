# Operator Specification: LaserGeomOp

**Family:** `GeomOp`  
**Type Name:** `LaserGeomOp`  
**Version:** 1.0  
**Phase Ownership:** Phase 15 (Epic 15.8)  

---

## 1. Description

`LaserGeomOp` receives 3D geometry or 2D vector curves, samples vertices into connected contours, performs galvo optimization (blanking point insertion, corner dwell delay), and outputs an optimized laser point path.

---

## 2. Pins

### Input Pins
| Pin Name | Type | Required | Description |
|---|---|---|---|
| `geometry` | `Geom` | Yes | Input 3D/2D geometry mesh to trace |

### Output Pins
| Pin Name | Type | Description |
|---|---|---|
| `output` | `Geom` | Optimized laser point path geometry |
| `out_points` | `Chan` | Raw ILDA point channel stream (`x, y, z, r, g, b, blank`) |

---

## 3. Parameters

| Parameter | Type | Default | Range | Description |
|---|---|---|---|---|
| `target_pps` | `int` | `30000` | `10000 .. 100000` | Target scanner points per second |
| `blank_delay` | `int` | `8` | `0 .. 32` | Blanked transit points between contours |
| `corner_dwell` | `int` | `4` | `0 .. 16` | Dwell points at acute angles |
| `color_override` | `vec4` | `[0.0, 1.0, 0.2, 1.0]` | - | Laser beam color |

---

## 4. Cook Semantics

- **Time-Dependent:** Yes.
