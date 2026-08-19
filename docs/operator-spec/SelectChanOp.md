# Operator Specification: SelectChanOp

**Family:** `ChanOp` (Channel Operator)  
**Name:** `Select`  
**Description:** Filters, selects, reorders, and renames channels from an incoming channel buffer using wildcard patterns.

---

## Inputs & Outputs

### Input Pins
- `input` (`PinType::Chan`): Source channel stream.

### Output Pins
- `output` (`PinType::Chan` / `nf::ChannelBuffer`): Filtered channel stream.

---

## Parameters

| Name | Type | Default | Description |
|---|---|---|---|
| `channel_names` | `std::string` | `"*"` | Space-separated list of channel names or wildcard patterns (e.g. `chan* tx ty tz`) |
| `rename_to` | `std::string` | `""` | Optional replacement names (e.g. `r g b` or `out*`) |

---

## Cook Behavior
1. Parses channel name selection list and pattern wildcards.
2. Matches incoming channels from `input`.
3. Applies renaming rules if specified.
4. Outputs subset buffer.
