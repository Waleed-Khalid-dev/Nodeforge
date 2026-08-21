# Operator Specification: VoxelGridGeomOp

**Family:** `GeomOp`  
**Type Name:** `VoxelGridGeomOp`  
**Version:** 1.0  
**Phase Ownership:** Phase 15 (Epic 15.7)  

---

## 1. Description

`VoxelGridGeomOp` computes a 3D scalar density field and outputs bounding volume wireframes and isosurface geometry for volumetric simulation.

---

## 2. Pins

### Input Pins
None.

### Output Pins
| Pin Name | Type | Description |
|---|---|---|
| `output` | `Geom` | Generated 3D bounding geometry and voxel slices |

---

## 3. Parameters

| Parameter | Type | Default | Range | Description |
|---|---|---|---|---|
| `grid_resolution` | `int` | `32` | `8 .. 128` | 3D voxel dimension ($N \times N \times N$) |
| `box_size` | `vec3` | `[10.0, 10.0, 10.0]` | - | Physical extent of bounding volume |
| `field_type` | `int` | `0` | `0 .. 2` | 0: Sphere Isosurface, 1: Fractal Noise, 2: Torus SDF |

---

## 4. Cook Semantics

- **Time-Dependent:** No.
