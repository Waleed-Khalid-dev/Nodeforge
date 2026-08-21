# Operator Specification: ParticleMatOp

**Family:** `MatOp`  
**Type Name:** `ParticleMatOp`  
**Version:** 1.0  
**Phase Ownership:** Phase 15 (Epic 15.1)  

---

## 1. Description

`ParticleMatOp` is a specialized visual material for rendering high-density particle clouds. It supports camera-facing billboard quads, hardware point sprites, custom texture maps, depth softening against opaque geometry, and additive or alpha blending.

---

## 2. Pins

### Input Pins
| Pin Name | Type | Required | Description |
|---|---|---|---|
| `texture` | `Tex` | No | Optional 2D texture sprite (e.g. soft circle, spark flare, star) |

### Output Pins
| Pin Name | Type | Description |
|---|---|---|
| `output` | `Mat` | Material data descriptor fed into `GeometryComp` or `RenderTexOp` |

---

## 3. Parameters

| Parameter | Type | Default | Range | Description |
|---|---|---|---|---|
| `blend_mode` | `int` | `0` | `0 .. 2` | 0: Additive (Glow), 1: Alpha Blend, 2: Screen |
| `point_type` | `int` | `0` | `0 .. 2` | 0: Point Sprite, 1: Camera Billboard Quad, 2: Motion Streak |
| `base_size` | `float` | `0.1` | `0.001 .. 10.0` | Global particle display size multiplier |
| `size_attenuation`| `bool` | `true` | `true/false` | Shrinks particle points with camera distance |
| `soft_falloff` | `bool` | `true` | `true/false` | Applies soft circular Gaussian fade to point edges |
| `depth_fade` | `float` | `0.2` | `0.0 .. 5.0` | Softens intersection edges against opaque scene meshes |
| `color_multiplier`| `vec4` | `[1.0, 1.0, 1.0, 1.0]`| `[0.0 .. 10.0]` | HDR color and intensity gain multiplier |

---

## 4. Cook Semantics

- **Time-Dependent:** No (unless driven by dynamic expressions).
- **Shader Integration:** Produces custom vertex and fragment shaders for `RenderTexOp`.
