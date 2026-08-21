# Operator Specification: InstanceGeomOp

**Family:** `GeomOp`  
**Type Name:** `InstanceGeomOp`  
**Version:** 1.0  
**Phase Ownership:** Phase 15 (Epic 15.2)  

---

## 1. Description

`InstanceGeomOp` duplicates a base 3D template geometry mesh across thousands to hundreds of thousands of instance locations. It supports 4 procedural distribution modes (Tables, Channels, Surface Normals, Grid Arrays, and Fibonacci Spirals) with per-instance transform variations, color ramps, and audio-reactive wave modulation.

---

## 2. Pins

### Input Pins
| Pin Name | Type | Required | Description |
|---|---|---|---|
| `template_mesh` | `Geom` | Yes | Base 3D geometry mesh (e.g. Box, Sphere, Torus, custom mesh) |
| `distribution_mesh` | `Geom` | No | Target surface mesh whose vertices and normals define instance positions and orientations |
| `instances_data` | `Data` | No | Optional 2D table containing instance transformation rows (`tx, ty, tz, scale, rot_x, rot_y, rot_z, color`) |
| `instances_chan` | `Chan` | No | Optional SIMD channels modulating instance transforms or colors |

### Output Pins
| Pin Name | Type | Description |
|---|---|---|
| `output` | `Geom` | Instanced geometry containing base template vertices and populated `InstanceData` array |

---

## 3. Parameters

| Parameter | Type | Default | Range | Description |
|---|---|---|---|---|
| `distribution_mode` | `int` | `2` | `0 .. 3` | 0: Table/Channel, 1: Mesh Surface, 2: Grid Array, 3: Fibonacci Spiral |
| `grid_count` | `ivec3` | `[30, 30, 1]` | `[1 .. 500]` | Number of instances along X, Y, and Z axes |
| `grid_spacing` | `vec3` | `[1.0, 1.0, 1.0]` | `[0.01 .. 100.0]` | Distance between instance grid points |
| `grid_center` | `bool` | `true` | `true/false` | Centers the grid array around the origin |
| `spiral_count` | `int` | `5000` | `10 .. 100000` | Total number of instances in spiral layout |
| `spiral_radius_scale`| `float`| `0.5` | `0.01 .. 10.0` | Radial growth rate multiplier |
| `spiral_twist` | `float` | `137.5077` | `0.0 .. 360.0` | Angular turn per instance in degrees (Golden Angle: 137.5°) |
| `noise_displacement`| `float`| `0.0` | `0.0 .. 50.0` | 3D Simplex jitter displacement amplitude |
| `noise_frequency` | `float` | `0.5` | `0.01 .. 10.0` | Spatial frequency of displacement noise |
| `align_to_normals` | `bool` | `true` | `true/false` | Orients instance local up vector to surface normal (Mode 1) |
| `color_mode` | `int` | `1` | `0 .. 3` | 0: Uniform, 1: Index Ramp, 2: Distance Ramp, 3: Table/Channel |
| `start_color` | `vec4` | `[0.1, 0.7, 1.0, 1.0]` | `[0.0 .. 1.0]` | Beginning gradient color |
| `end_color` | `vec4` | `[1.0, 0.2, 0.6, 1.0]` | `[0.0 .. 1.0]` | Ending gradient color |
| `instance_scale` | `vec3` | `[1.0, 1.0, 1.0]` | `[0.001 .. 10.0]` | Global scale multiplier for individual instances |
| `audio_scale_mod` | `float` | `1.0` | `0.0 .. 10.0` | Audio-reactive scaling amplitude multiplier |

---

## 4. Cook Semantics

- **Time-Dependent:** Only if animated via expressions, time parameters, or dynamic channel inputs.
- **Resource Management:** Populates `GeometryData::GetInstances()` with zero CPU mesh duplication.
