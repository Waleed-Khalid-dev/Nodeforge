# Operator Specification: ResolutionTexOp

**Family:** `TexOp` (Texture Operator)  
**Name:** `Resolution`  
**Description:** Forces a specific resolution on an input texture with configurable aspect-ratio fitting modes.

---

## Inputs & Outputs

### Input Pins
- `input` (`PinType::Tex`): Input image texture.

### Output Pins
- `output` (`PinType::Tex`): Resampled texture at the target resolution.

---

## Parameters

| Name | Type | Default | Range / Description |
|---|---|---|---|
| `resolution_w` | `int32_t` | 1920 | [1, 8192] Target width in pixels |
| `resolution_h` | `int32_t` | 1080 | [1, 8192] Target height in pixels |
| `fit_mode` | `int32_t` | 0 | 0: Stretch, 1: Fit Horizontal, 2: Fit Vertical, 3: Fill/Crop, 4: Native/Center |

---

## Cook Behavior
1. Leases a render target of dimensions `(resolution_w, resolution_h)` from `gpu::TexturePool`.
2. Computes the UV transform matrix based on input aspect ratio vs target aspect ratio and `fit_mode`.
3. Renders the rescaled texture using bilinear hardware filtering into the target buffer.
