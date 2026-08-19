# OSCInOp

## Overview
`OSCInOp` receives UDP Open Sound Control packets over a network port via a background thread, decoding messages into a timestamped multi-row table.

## Category
- **Family:** `DataOp` (DAT)
- **C++ Class:** `nf::OSCInOp`
- **Output Type:** `PinType::Data` (`DataTable`)

## Parameters

| Parameter | Type | Default | Min | Max | Description |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `port` | `Int` | `7000` | 1024 | 65535 | Local UDP port to listen on. |
| `active` | `Bool` | `true` | - | - | Enable/disable UDP receiver thread. |
| `max_rows` | `Int` | `100` | 1 | 10000 | Maximum history rows to retain. |
| `clear_on_cook` | `Bool` | `false` | - | - | Retain only new messages arrived since last frame. |

## Table Schema
Columns: `timestamp`, `address`, `args`

## Inputs / Outputs
- **Inputs:** None (Network Receiver).
- **Outputs:**
  - `output` (`PinType::Data`): Received OSC message log `DataTable`.
