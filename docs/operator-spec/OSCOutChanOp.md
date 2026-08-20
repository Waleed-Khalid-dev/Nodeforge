# OSCOutChanOp

## Overview
`OSCOutChanOp` bundles input channels into Open Sound Control (OSC) UDP packets and broadcasts or unicasts them to remote network hosts at the graph cook rate or upon value change.

## Category
- **Family:** `ChanOp` (CHOP)
- **C++ Class:** `nf::OSCOutChanOp`
- **Output Type:** `PinType::Channel` (`ChannelBuffer` passthrough)

## Parameters

| Parameter | Type | Default | Min | Max | Description |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `host` | `String` | `"127.0.0.1"` | - | - | Destination IP address or hostname. |
| `port` | `Int` | `8000` | 1024 | 65535 | Destination UDP port. |
| `address_prefix` | `String` | `"/nf"` | - | - | OSC address prefix (e.g. `/nf` produces `/nf/chan_name`). |
| `bundle_mode` | `Bool` | `true` | - | - | Transmit all channels in a single `#bundle` or separate packets. |
| `send_on_change_only` | `Bool` | `false` | - | - | Only send packets when input channel values change. |
| `active` | `Bool` | `true` | - | - | Enable/disable UDP transmission. |

## Inputs / Outputs
- **Inputs:**
  - `input` (`PinType::Channel`): Channels to encode and transmit.
- **Outputs:**
  - `output` (`PinType::Channel`): Passthrough channel buffer.
