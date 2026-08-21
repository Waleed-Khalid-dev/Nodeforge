# GeomOp Family Cheat Sheet (3D Meshes, Primitives & SOPs)

**Family:** `GeomOp` | **Color:** Blue (`#2979FF`) | **Data Structure:** `GeometryData` (Interleaved Vertex/Index Mesh Buffers)

---

## 1. GeomOp Architecture

GeomOps generate, transform, deform, and merge 3D surface geometry models on the CPU before uploading to GPU vertex and index buffers for rendering.

---

## 2. Core Operator Reference (10 Operators)

| Operator | In Pins | Out Pins | Key Parameters | Cook Behavior |
|----------|---------|----------|----------------|---------------|
| **`GridGeomOp`** | None | `output` (Geom) | `size_x`, `size_y`, `rows`, `cols` | Generates a 2D/3D planar quad mesh grid |
| **`SphereGeomOp`** | None | `output` (Geom) | `radius`, `rows` (rings), `cols` (segments) | Generates UV sphere mesh |
| **`BoxGeomOp`** | None | `output` (Geom) | `size_x`, `size_y`, `size_z`, `div_x`, `div_y`, `div_z` | Generates 6-sided 3D cube mesh with UVs |
| **`TorusGeomOp`** | None | `output` (Geom) | `major_radius`, `minor_radius`, `major_segments`, `minor_segments` | Generates donut torus geometry |
| **`CylinderGeomOp`**| None | `output` (Geom) | `radius`, `height`, `segments`, `capped` (bool) | Generates open/capped cylinder mesh |
| **`TransformGeomOp`**| `input` (Geom) | `output` (Geom) | `translate` (vec3), `rotate` (vec3), `scale` (vec3) | Multiplies mesh vertices by 4x4 affine matrix |
| **`MergeGeomOp`** | `input_a`, `input_b` | `output` (Geom) | None | Merges multiple mesh buffers into a single draw call |
| **`NoiseDeformGeomOp`**| `input` (Geom) | `output` (Geom) | `frequency` (float), `amplitude` (float), `time` (float), `direction` (Normal/XYZ) | Displaces mesh vertices using 3D Simplex noise |
| **`NormalsGeomOp`** | `input` (Geom) | `output` (Geom) | `mode` (Flat / Smooth / Face-Area Weighted) | Computes smooth per-vertex lighting normals |
| **`ChanToGeomOp`** | `input` (Chan) | `output` (Geom) | `channel_mapping` (x y z nx ny nz) | Generates point cloud or polyline from channels |

---

## 3. Common Idioms

```
# Procedural Deformed Terrain
GridGeomOp (Rows: 50, Cols: 50) ──► NoiseDeformGeomOp (Amp: 2.5) ──► NormalsGeomOp (Smooth) ──► GeometryComp

# Merged Architectural Scene
GridGeomOp (Floor) ──┬──► MergeGeomOp ──► GeometryComp ──► RenderTexOp
BoxGeomOp (Pillars) ─┘
```
