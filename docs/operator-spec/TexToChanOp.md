# Operator Specification: TexToChanOp

**Family:** `ChanOp` (Channel Operator) / `TexOp` Interop  
**Name:** `TexToChan` (TopToChop)  
**Description:** Reads pixel data from a GPU Texture2D back into CPU ChannelBuffers for audio synthesis, luminance tracking, motion telemetry, and channel-driven parameter feedback.

---

## Inputs & Outputs

### Input Pins
- `input` (`PinType::Texture` / `gpu::Texture2D`): Source GPU texture.

### Output Pins
- `output` (`PinType::Chan` / `nf::ChannelBuffer`): Extracted multi-channel buffer (channels `r`, `g`, `b`, `a`).

---

## Parameters

| Name | Type | Default | Description |
|---|---|---|---|
| `sample_mode` | `int32_t` (Menu) | 0 | `0: Single Scanline (Row Index)`, `1: Average All Pixels (1 Sample)`, `2: All Pixels 1D` |
| `row_index` | `int32_t` | 0 | Scanline row index when using Scanline mode |
| `max_samples` | `int32_t` | 1024 | Maximum samples to read back |

---

## Cook Behavior
1. Queries source `Texture2D` from `input`.
2. Reads back pixel data into staging host-visible buffer.
3. Decodes RGBA color components into separate planar channel arrays (`r`, `g`, `b`, `a`).
4. Outputs `ChannelBuffer` on `output` pin.
