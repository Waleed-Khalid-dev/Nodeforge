# Operator Specification: ConstantChanOp

**Family:** `ChanOp` (Channel Operator)  
**Name:** `Constant`  
**Description:** Generates constant multi-channel numerical streams (scalars, vectors, or arrays of samples).

---

## Inputs & Outputs

### Input Pins
- None (Generator Node)

### Output Pins
- `output` (`PinType::Chan` / `nf::ChannelBuffer`): Multi-channel numeric buffer.

---

## Parameters

| Name | Type | Default | Description |
|---|---|---|---|
| `channel_names` | `std::string` | `"chan1 chan2"` | Space-separated list of channel names |
| `values` | `glm::vec4` | `(1.0, 0.0, 0.0, 0.0)` | Initial values for channels |
| `sample_rate` | `float` | 60.0 | Sample rate in Hz |
| `sample_count` | `int32_t` | 1 | Number of samples per channel |

---

## Cook Behavior
1. Parses channel names from parameter string.
2. Allocates `ChannelBuffer` with specified number of channels, sample rate, and sample length.
3. Fills channel arrays with the parameter values.
4. Writes payload to the `output` pin.
