# OSCInChanOp

## Overview
`OSCInChanOp` receives UDP Open Sound Control (OSC) packets asynchronously and extracts float/int arguments into named real-time channels for high-speed parameter modulation and stage control.

## Category
- **Family:** `ChanOp` (CHOP)
- **C++ Class:** `nf::OSCInChanOp`
- **Output Type:** `PinType::Channel` (`ChannelBuffer`)

## Parameters

| Parameter | Type | Default | Min | Max | Description |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `port` | `Int` | `7000` | 1024 | 65535 | Local UDP port to listen for OSC messages. |
| `address_filter` | `String` | `""` | - | - | Optional OSC address prefix filter (e.g. `/show/*`). |
| `decay_rate` | `Float` | `0.0` | 0.0 | 10.0 | Exponential decay rate per second towards 0 for pulse addresses. |
| `active` | `Bool` | `true` | - | - | Enable/disable UDP receiver thread. |

## Channel Naming Rules
- OSC address `/speed` $\rightarrow$ channel `speed`
- OSC address `/light/intensity` $\rightarrow$ channel `light_intensity`
- Multi-argument address `/pos` with `[1.0, 2.5, 3.2]` $\rightarrow$ channels `pos_1`, `pos_2`, `pos_3`

## Outputs
- `output` (`PinType::Channel`): Channel buffer with all active OSC address channels.
