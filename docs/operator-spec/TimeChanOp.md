# Operator Specification: TimeChanOp

**Family:** `ChanOp` (Channel Operator)  
**Name:** `Time`  
**Description:** Generates continuous timeline, clock, frame index, and playback progress channels.

---

## Inputs & Outputs

### Input Pins
- None (Generator Node)

### Output Pins
- `output` (`PinType::Chan` / `nf::ChannelBuffer`): Multi-channel buffer outputting time metrics.

---

## Parameters

| Name | Type | Default | Description |
|---|---|---|---|
| `output_seconds` | `bool` | `true` | Include `seconds` channel (current time in seconds) |
| `output_frames` | `bool` | `true` | Include `frame` channel (integer frame counter) |
| `output_fraction` | `bool` | `true` | Include `fraction` channel (0.0 to 1.0 timeline loop progress) |
| `output_rate` | `bool` | `false` | Include `rate` channel (current FPS) |
| `sample_rate` | `float` | 60.0 | Sample rate in Hz |

---

## Cook Behavior
1. Queries `CookContext` for current frame, timeline seconds, playback range, and delta time.
2. Allocates a `ChannelBuffer` with active time channels.
3. Populates channel values with continuous time data.
4. Outputs buffer on `output` pin.
