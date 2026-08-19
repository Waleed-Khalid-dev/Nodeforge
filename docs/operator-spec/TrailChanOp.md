# Operator Specification: TrailChanOp

**Family:** `ChanOp` (Channel Operator)  
**Name:** `Trail`  
**Description:** Records a continuous sliding ring buffer of historical samples from real-time time-sliced channel inputs.

---

## Inputs & Outputs

### Input Pins
- `input` (`PinType::Chan`): Live time-sliced channel stream.
- `reset` (`PinType::Chan`, Optional): Clears historical buffer.

### Output Pins
- `output` (`PinType::Chan` / `nf::ChannelBuffer`): Multi-sample historical waveform buffer.

---

## Parameters

| Name | Type | Default | Description |
|---|---|---|---|
| `length` | `float` | 4.0 | History window length in seconds |
| `max_samples` | `int32_t` | 240 | Maximum recorded samples per channel |
| `capture` | `bool` | `true` | Enable live sample capturing |

---

## Cook Behavior
1. Appends latest incoming samples from `input` to persistent per-channel ring buffers.
2. Truncates older samples beyond `max_samples` or `length * sample_rate`.
3. Flattens ring buffer into contiguous `ChannelBuffer` on `output` pin.
