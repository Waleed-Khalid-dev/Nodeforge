# Operator Specification: ChanToTexOp

**Family:** `TexOp` (Texture Operator) / `ChanOp` Interop  
**Name:** `ChanToTex` (ChopToTop)  
**Description:** Converts numeric multi-channel buffer data into 1D/2D GPU textures (RGBA32F or RGBA8) for shader modulation, displacement maps, audio visualizers, and lookup tables.

---

## Inputs & Outputs

### Input Pins
- `input` (`PinType::Chan` / `nf::ChannelBuffer`): Source channel stream.

### Output Pins
- `output` (`PinType::Texture` / `gpu::Texture2D`): Generated GPU texture.

---

## Parameters

| Name | Type | Default | Description |
|---|---|---|---|
| `data_format` | `int32_t` (Menu) | 0 | `0: RGBA32F`, `1: R32F`, `2: RGBA8` |
| `fit_method` | `int32_t` (Menu) | 0 | `0: 1D Line (Width = Samples, Height = 1)`, `1: 2D Grid (Width x Height)`, `2: Channel per Row` |
| `normalize` | `bool` | `false` | Normalize channel values to 0.0 .. 1.0 range |

---

## Cook Behavior
1. Reads incoming `ChannelBuffer` from `input`.
2. Maps channels to texture color components (Channel 0 -> R, Channel 1 -> G, Channel 2 -> B, Channel 3 -> A).
3. Uploads packed pixel buffer into GPU `Texture2D`.
4. Outputs texture handle on `output` pin.
