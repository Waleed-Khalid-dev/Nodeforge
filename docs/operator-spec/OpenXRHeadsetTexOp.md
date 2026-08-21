# Operator Specification: OpenXRHeadsetTexOp

**Family:** `TexOp`  
**Type Name:** `OpenXRHeadsetTexOp`  
**Version:** 1.0  
**Phase Ownership:** Phase 15 (Epic 15.6)  

---

## 1. Description

`OpenXRHeadsetTexOp` receives left and right stereoscopic eye render textures, presents them to the connected OpenXR VR/AR head-mounted display, and outputs a 2D companion preview texture.

---

## 2. Pins

### Input Pins
| Pin Name | Type | Required | Description |
|---|---|---|---|
| `in_left_eye` | `Tex` | Yes | Left eye rendered texture |
| `in_right_eye` | `Tex` | No | Optional right eye rendered texture (if omitted, left eye is duplicated) |

### Output Pins
| Pin Name | Type | Description |
|---|---|---|
| `output` | `Tex` | 2D desktop companion preview texture (side-by-side or dominant eye) |

---

## 3. Parameters

| Parameter | Type | Default | Range | Description |
|---|---|---|---|---|
| `stereo_mode` | `int` | `0` | `0 .. 1` | 0: Side-by-Side Preview, 1: Dominant Eye Preview |
| `ipd_mm` | `float` | `64.0` | `50.0 .. 80.0` | Interpupillary distance in millimeters |
| `mirror_window` | `bool` | `true` | `true/false` | Mirror output to companion desktop window |
| `enable_passthrough` | `bool` | `false` | `true/false` | Enable AR/MR camera passthrough background |

---

## 4. Cook Semantics

- **Time-Dependent:** Yes.
