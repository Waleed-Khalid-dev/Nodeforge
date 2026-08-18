# Operator Specification: ConstantTexOp

**Family:** `TexOp` (Texture Operator)  
**Name:** `Constant`  
**Description:** Generates a solid 2D texture filled with a constant color and resolution.

---

## Inputs & Outputs

### Input Pins
- None (Generator Node)

### Output Pins
- `output` (`PinType::Tex` / `std::shared_ptr<gpu::Texture2D>`): The generated solid color texture.

---

## Parameters

| Name | Type | Default | Range / Description |
|---|---|---|---|
| `resolution_w` | `int32_t` | 1280 | [1, 8192] Width in pixels |
| `resolution_h` | `int32_t` | 720 | [1, 8192] Height in pixels |
| `color` | `glm::vec4` | `(0.2, 0.4, 0.8, 1.0)` | RGBA normalized color values `[0.0, 1.0]` |

---

## Cook Behavior
1. Checks if texture already exists with identical dimensions and color.
2. If dirty or dimensions changed, allocates a 2D GPU image with `VK_FORMAT_R8G8B8A8_UNORM` via VMA.
3. Fills pixels and uploads via staging buffer transfer command.
4. Sets the resulting `gpu::Texture2D` shared pointer to the `output` pin.
