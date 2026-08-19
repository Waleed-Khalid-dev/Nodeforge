# Operator Specification: MergeChanOp

**Family:** `ChanOp` (Channel Operator)  
**Name:** `Merge`  
**Description:** Merges multiple incoming channel buffers into a single unified multi-channel stream.

---

## Inputs & Outputs

### Input Pins
- `input1` (`PinType::Chan`): First channel buffer.
- `input2` (`PinType::Chan`): Second channel buffer.
- `input3` (`PinType::Chan`, Optional): Third channel buffer.
- `input4` (`PinType::Chan`, Optional): Fourth channel buffer.

### Output Pins
- `output` (`PinType::Chan` / `nf::ChannelBuffer`): Merged multi-channel buffer.

---

## Parameters

| Name | Type | Default | Description |
|---|---|---|---|
| `duplicate_names` | `int32_t` (Menu) | 0 | Conflict handling: `0: Replace (Keep Last)`, `1: Append Suffix (_1, _2)`, `2: Error` |
| `align_length` | `int32_t` (Menu) | 0 | Length alignment: `0: Pad with Zero/Hold`, `1: Truncate to Shortest`, `2: Resample` |

---

## Cook Behavior
1. Iterates over all connected input pins.
2. Collects channels and validates sample counts and rates.
3. Resolves name collisions per `duplicate_names` policy.
4. Constructs and outputs unified `ChannelBuffer`.
