# Operator Specification: BlurTexOp

**Family:** `TexOp` (Texture Operator)  
**Name:** `Blur`  
**Description:** High-performance two-pass separable Gaussian blur filter running on GPU.

---

## Inputs & Outputs

### Input Pins
- `input` (`PinType::Tex`): Input image texture.

### Output Pins
- `output` (`PinType::Tex`): Gaussian blurred image texture.

---

## Parameters

| Name | Type | Default | Range / Description |
|---|---|---|---|
| `radius` | `float` | 4.0f | [0.0, 64.0] Gaussian standard deviation / kernel radius |
| `passes` | `int32_t` | 1 | [1, 4] Iterative filter passes |

---

## Cook Behavior
1. If input is missing or radius is 0, forwards input directly.
2. Leases an intermediate ping-pong texture and a final render target from `gpu::TexturePool`.
3. Pass 1 (Horizontal): Samples input with 1D Gaussian kernel along the X-axis into the intermediate texture.
4. Pass 2 (Vertical): Samples intermediate texture with 1D Gaussian kernel along the Y-axis into the output texture.
5. Sets final blurred texture on output pin.
