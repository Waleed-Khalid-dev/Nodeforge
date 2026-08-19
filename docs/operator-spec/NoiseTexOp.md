# Operator Specification: NoiseTexOp

**Family:** `TexOp` (Texture Operator)  
**Name:** `Noise`  
**Description:** Procedural 2D Perlin / Simplex / Value noise and Fractal Brownian Motion (FBM) generator on GPU.

---

## Inputs & Outputs

### Input Pins
- None (Generator Node)

### Output Pins
- `output` (`PinType::Tex`): Generated procedural 2D noise texture.

---

## Parameters

| Name | Type | Default | Range / Description |
|---|---|---|---|
| `resolution_w` | `int32_t` | 1280 | [1, 8192] Width in pixels |
| `resolution_h` | `int32_t` | 720 | [1, 8192] Height in pixels |
| `noise_type` | `int32_t` | 0 | 0: Perlin, 1: Simplex, 2: Value |
| `period` | `float` | 1.0f | Spatial frequency / scaling factor |
| `octaves` | `int32_t` | 3 | [1, 8] Fractal octave detail |
| `roughness` | `float` | 0.5f | [0.0, 1.0] Harmonic amplitude decay |
| `translate_x` | `float` | 0.0f | X offset in noise coordinate space |
| `translate_y` | `float` | 0.0f | Y offset in noise coordinate space |
| `translate_z` | `float` | 0.0f | Z (time) animated phase offset |

---

## Cook Behavior
1. Leases a `(resolution_w, resolution_h, VK_FORMAT_R8G8B8A8_UNORM)` texture from `gpu::TexturePool`.
2. Dispatches a 2D Compute pass (`vkCmdDispatch`) evaluating GPU noise algorithms per pixel.
3. Sets the resulting texture on the output pin.
