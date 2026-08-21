# Operator Specification: LightShaftTexOp

**Family:** `TexOp`  
**Type Name:** `LightShaftTexOp`  
**Version:** 1.0  
**Phase Ownership:** Phase 15 (Epic 15.7)  

---

## 1. Description

`LightShaftTexOp` generates screen-space volumetric god-ray light shafts and crepuscular beams using multi-sample radial blur attenuation.

---

## 2. Pins

### Input Pins
| Pin Name | Type | Required | Description |
|---|---|---|---|
| `input` | `Tex` | Yes | Source rendered scene color texture |
| `in_occlusion` | `Tex` | No | Optional occlusion silhouette mask |

### Output Pins
| Pin Name | Type | Description |
|---|---|---|
| `output` | `Tex` | Scene texture with additive volumetric light shafts |

---

## 3. Parameters

| Parameter | Type | Default | Range | Description |
|---|---|---|---|---|
| `light_pos_screen` | `vec2` | `[0.5, 0.5]` | `[0.0, 0.0] .. [1.0, 1.0]` | Light source 2D screen coordinate |
| `density` | `float` | `0.8` | `0.0 .. 2.0` | Light ray density |
| `decay` | `float` | `0.95` | `0.5 .. 1.0` | Radial falloff decay factor |
| `weight` | `float` | `0.5` | `0.0 .. 2.0` | Sample intensity weight |
| `samples` | `int` | `32` | `8 .. 64` | Radial ray sample count |

---

## 4. Cook Semantics

- **Time-Dependent:** No.
