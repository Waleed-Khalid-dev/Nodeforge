# Operator Specification: MathChanOp

**Family:** `ChanOp` (Channel Operator)  
**Name:** `Math`  
**Description:** Performs channel-wise mathematical and arithmetic transformations (addition, multiplication, trigonometric operations, and range remapping).

---

## Inputs & Outputs

### Input Pins
- `input` (`PinType::Chan` / `nf::ChannelBuffer`): Source channels.

### Output Pins
- `output` (`PinType::Chan` / `nf::ChannelBuffer`): Processed channel buffer.

---

## Parameters

| Name | Type | Default | Description |
|---|---|---|---|
| `operation` | `int32_t` | 0 | 0: Add, 1: Multiply, 2: Sin, 3: Cos, 4: Abs, 5: Clamp |
| `scalar` | `float` | 1.0 | Operand value for arithmetic operations |
| `range_in_min` | `float` | 0.0 | Input range lower bound |
| `range_in_max` | `float` | 1.0 | Input range upper bound |
| `range_out_min` | `float` | 0.0 | Output mapped range lower bound |
| `range_out_max` | `float` | 1.0 | Output mapped range upper bound |

---

## Cook Behavior
1. Validates that `input` contains a valid `ChannelBuffer`. If not, outputs an empty buffer.
2. Iterates over all channels and samples, applying the selected mathematical operation and range remap.
3. Preserves channel names and sample rate in the resulting `output` buffer.
