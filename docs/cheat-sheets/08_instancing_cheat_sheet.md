# GPU Instancing Subsystem Cheat Sheet

**Subsystem:** GPU Instancing Engine | **Version:** 1.0 (Phase 15 Epic 15.2) | **Family:** `GeomOp`

---

## 1. InstanceGeomOp Reference

| Input Pin | Type | Required | Description |
|---|---|---|---|
| `template_mesh` | `Geom` | **Yes** | Base 3D geometry mesh to duplicate (Box, Sphere, Torus, custom mesh) |
| `distribution_mesh` | `Geom` | No | Target surface mesh for vertex/normal distribution |
| `instances_data` | `Data` | No | 2D Table containing explicit instance coordinates/attributes |
| `instances_chan` | `Chan` | No | SIMD Channels modulating instance matrices or colors |

---

## 2. Distribution Topologies

| Mode | ID | Description | Typical Use Case |
|---|---|---|---|
| **Table / Channel** | `0` | Direct 1-to-1 mapping from columns/channels | Architectural asset placement, point clouds |
| **Mesh Surface** | `1` | Instances placed at each vertex, oriented to normal $\vec{N}$ | Crystals on terrain, lights on sphere |
| **Grid Array** | `2` | Parametric 1D/2D/3D matrix array ($N_x, N_y, N_z$) | Kinetic LED matrix, holographic cube arrays |
| **Fibonacci Spiral** | `3` | Golden angle phyllotaxis ($\theta = 137.5^{\circ}$, $r = c\sqrt{n}$) | Organic floral swarms, dome sculptures |

---

## 3. Standard Network Wiring Idiom

```
# Audio-Reactive 100,000-Object Kinetic Matrix
BoxGeomOp (Template Mesh: [0.2, 0.2, 0.2])
       │
       ▼
InstanceGeomOp ◄── AudioFileInChanOp (Spectrum Channels)
  (Mode: Grid Array [100, 100, 1])
       │
       ▼
GeometryComp ◄── [PhongMatOp] (Specular Blinn-Phong)
       │
       ▼
  RenderTexOp ──► BlurTexOp (Bloom Glow) ──► ProjectorOut
```
