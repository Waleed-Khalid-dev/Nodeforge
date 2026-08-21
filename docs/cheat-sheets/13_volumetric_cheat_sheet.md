# Volumetric Fog & Ray Marching Subsystem Cheat Sheet

**Subsystem:** Volumetric Fog & Real-Time GPU Ray Marching | **Version:** 1.0 (Phase 15 Epic 15.7)

---

## 1. Volumetric Operator Reference

| Operator | Family | In Pins | Out Pins | Key Roles |
|---|---|---|---|---|
| **`VolumetricCloudTexOp`** | `TexOp` | `in_env` (Tex, opt) | `output` (Tex) | Full-screen ray-marched atmospheric clouds & sky generator |
| **`VolumetricFogMatOp`** | `MatOp` | None | `output` (Mat) | 3D volume material for localized smoke plumes & fog zones |
| **`VoxelGridGeomOp`** | `GeomOp` | None | `output` (Geom) | 3D scalar voxel field generator & isosurfaces |
| **`LightShaftTexOp`** | `TexOp` | `input` (Tex), `in_occlusion` (Tex, opt) | `output` (Tex) | Screen-space volumetric god-ray light shafts |

---

## 2. Standard Network Wiring Idiom

```
# Atmospheric Cathedral with Volumetric Light Shafts & Audio Reactive Clouds
VolumetricCloudTexOp (Sun Clouds) ──► Scene3DPass (Cathedral Geometry) ──► LightShaftTexOp (God-Rays)
                                            ▲
BoxGeomOp ──► GeometryComp ────────────────┘
                  ▲
VolumetricFogMatOp (Smoke Plume)
```
