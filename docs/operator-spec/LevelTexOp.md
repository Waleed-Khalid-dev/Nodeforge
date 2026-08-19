# Operator Specification: LevelTexOp

**Family:** `TexOp` (Texture Operator)  
**Name:** `Level`  
**Description:** Photometric image processing adjusting black level, white level, brightness, contrast, and gamma.

---

## Inputs & Outputs

### Input Pins
- `input` (`PinType::Tex`): Input image texture.

### Output Pins
- `output` (`PinType::Tex`): Color-adjusted image texture.

---

## Parameters

| Name | Type | Default | Range / Description |
|---|---|---|---|
| `black_level` | `float` | 0.0f | [0.0, 1.0] Input black point cutoff |
| `white_level` | `float` | 1.0f | [0.0, 1.0] Input white point cutoff |
| `brightness` | `float` | 1.0f | [0.0, 4.0] Linear brightness multiplier |
| `gamma` | `float` | 1.0f | [0.1, 5.0] Non-linear gamma exponent |
| `contrast` | `float` | 1.0f | [0.0, 4.0] Contrast expansion around mid-gray |

---

## Cook Behavior
1. Formula applied per pixel:
   $$\text{val} = \text{clamp}\left(\frac{\text{color} - \text{black\_level}}{\text{white\_level} - \text{black\_level}}, 0.0, 1.0\right)$$
   $$\text{val} = \text{pow}(\text{val}, 1.0 / \text{gamma})$$
   $$\text{val} = (\text{val} - 0.5) \times \text{contrast} + 0.5$$
   $$\text{color}_{\text{out}} = \text{val} \times \text{brightness}$$
2. Evaluated in a single dynamic rendering fragment pass into a pooled render target.
