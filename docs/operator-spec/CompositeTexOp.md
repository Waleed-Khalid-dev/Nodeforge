# Operator Specification: CompositeTexOp

**Family:** `TexOp` (Texture Operator)  
**Name:** `Composite`  
**Description:** Blends two input textures together using configurable layer blending operations.

---

## Inputs & Outputs

### Input Pins
- `input1` (`PinType::Tex`): Base layer texture.
- `input2` (`PinType::Tex`): Overlay layer texture.

### Output Pins
- `output` (`PinType::Tex`): Composite output texture.

---

## Parameters

| Name | Type | Default | Range / Description |
|---|---|---|---|
| `operation` | `int32_t` | 0 | 0: Over, 1: Add, 2: Multiply, 3: Subtract, 4: Screen, 5: Darken, 6: Lighten |
| `opacity` | `float` | 1.0f | [0.0, 1.0] Layer 2 blend strength |

---

## Cook Behavior
1. Validates `input1` and `input2`. If either input is missing, passes through the valid one or outputs black.
2. Leases a render target matching `input1` dimensions from `gpu::TexturePool`.
3. Records a dynamic rendering raster pass sampling both textures and applying the selected blend equation in the fragment shader.
4. Outputs the blended texture.
