# Operator Specification: VolumetricFogMatOp

**Family:** `MatOp`  
**Type Name:** `VolumetricFogMatOp`  
**Version:** 1.0  
**Phase Ownership:** Phase 15 (Epic 15.7)  

---

## 1. Description

`VolumetricFogMatOp` applies a 3D volumetric participating medium shader to 3D bounding geometry (boxes, spheres, cylinders), rendering localized smoke plumes, toxic gas clouds, or ground fog in `Scene3DPass`.

---

## 2. Pins

### Input Pins
None.

### Output Pins
| Pin Name | Type | Description |
|---|---|---|
| `output` | `Mat` | Volumetric material descriptor for `GeometryComp` |

---

## 3. Parameters

| Parameter | Type | Default | Range | Description |
|---|---|---|---|---|
| `density_scale` | `float` | `2.0` | `0.0 .. 50.0` | Internal density multiplier |
| `fog_color` | `vec4` | `[0.7, 0.8, 0.9, 1.0]` | - | Albedo scattering color |
| `noise_frequency` | `float` | `1.5` | `0.1 .. 20.0` | 3D turbulence frequency |
| `absorption` | `float` | `0.5` | `0.0 .. 5.0` | Light absorption coefficient |

---

## 4. Cook Semantics

- **Time-Dependent:** No.
