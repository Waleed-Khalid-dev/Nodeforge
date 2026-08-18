# Operator Specification: TransformTexOp

**Family:** `TexOp` (Texture Operator)  
**Name:** `Transform`  
**Description:** Applies 2D geometric transformations (translation, rotation, uniform/non-uniform scale, and pivot offset) to an input texture.

---

## Inputs & Outputs

### Input Pins
- `input` (`PinType::Tex` / `std::shared_ptr<gpu::Texture2D>`): The source texture to transform.

### Output Pins
- `output` (`PinType::Tex` / `std::shared_ptr<gpu::Texture2D>`): The transformed texture result.

---

## Parameters

| Name | Type | Default | Range / Description |
|---|---|---|---|
| `translate` | `glm::vec2` | `(0.0, 0.0)` | Normalized UV offset `[-10.0, 10.0]` |
| `rotate` | `float` | `0.0` | Rotation angle in degrees `[-360.0, 360.0]` |
| `scale` | `glm::vec2` | `(1.0, 1.0)` | Scale multipliers `[0.001, 100.0]` |
| `pivot` | `glm::vec2` | `(0.5, 0.5)` | Transformation pivot center in UV space |

---

## Cook Behavior
1. Requires a valid input texture on `input` pin. If empty, outputs `nullptr`.
2. Computes the inverse 3x3 affine transformation matrix for UV sampling.
3. In Phase 2 unit harness, forwards or scales the input buffer. In GPU render pipeline, executes fullscreen triangle pass with transform matrix push constant.
