# Operator Specification: NullTexOp

**Family:** `TexOp` (Texture Operator)  
**Name:** `Null`  
**Description:** Zero-cost pass-through alias node used for referencing texture streams and isolating graph dependencies.

---

## Inputs & Outputs

### Input Pins
- `input` (`PinType::Tex`): Upstream texture connection.

### Output Pins
- `output` (`PinType::Tex`): Forwarded texture reference identical to the input.

---

## Parameters

| Name | Type | Default | Description |
|---|---|---|---|
| `pass_through` | `bool` | `true` | When true, directly aliases the upstream texture. |

---

## Cook Behavior
1. If input is connected and has a valid texture, forwards the upstream `gpu::Texture2D` handle directly to `output` with zero copy overhead.
2. If input is disconnected, clears the output pin value.
