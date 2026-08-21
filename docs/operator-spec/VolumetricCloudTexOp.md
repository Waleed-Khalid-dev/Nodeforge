# Operator Specification: VolumetricCloudTexOp

**Family:** `TexOp`  
**Type Name:** `VolumetricCloudTexOp`  
**Version:** 1.0  
**Phase Ownership:** Phase 15 (Epic 15.7)  

---

## 1. Description

`VolumetricCloudTexOp` generates real-time procedural 3D ray-marched atmospheric clouds, sky fog, and celestial nebulas with Beer-Lambert extinction and Henyey-Greenstein scattering.

---

## 2. Pins

### Input Pins
| Pin Name | Type | Required | Description |
|---|---|---|---|
| `in_env` | `Tex` | No | Optional background sky / environment texture |

### Output Pins
| Pin Name | Type | Description |
|---|---|---|
| `output` | `Tex` | Rendered 3D volumetric cloud and atmospheric sky texture |

---

## 3. Parameters

| Parameter | Type | Default | Range | Description |
|---|---|---|---|---|
| `density` | `float` | `1.0` | `0.0 .. 10.0` | Cloud density scale |
| `sun_dir` | `vec3` | `[0.5, 0.8, -0.3]` | - | Sun light direction vector |
| `sun_color` | `vec4` | `[1.0, 0.9, 0.7, 1.0]` | - | Sun illumination color and intensity |
| `scattering_g` | `float` | `0.6` | `-0.9 .. 0.9` | Henyey-Greenstein forward scattering asymmetry |
| `step_count` | `int` | `32` | `8 .. 128` | Primary ray march steps |
| `wind_speed` | `float` | `0.5` | `-10.0 .. 10.0`| Procedural noise drift speed |

---

## 4. Cook Semantics

- **Time-Dependent:** Yes.
