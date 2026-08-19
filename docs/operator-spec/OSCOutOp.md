# OSCOutOp

## Overview
`OSCOutOp` transmits UDP Open Sound Control packets to a target IP address and port upon parameter changes or table row triggers.

## Category
- **Family:** `DataOp` (DAT)
- **C++ Class:** `nf::OSCOutOp`
- **Output Type:** `PinType::Data` (`DataTable`)

## Parameters

| Parameter | Type | Default | Min | Max | Description |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `ip_address` | `String` | `"127.0.0.1"` | - | - | Destination IP address or hostname. |
| `port` | `Int` | `7001` | 1024 | 65535 | Destination UDP port. |
| `address` | `String` | `"/nodeforge/trigger"` | - | - | OSC path address. |
| `value` | `Float` | `1.0` | - | - | Scalar value to send. |
| `send_on_cook` | `Bool` | `false` | - | - | Send packet on every cook. |
| `pulse_send` | `Bool` | `false` | - | - | Send packet immediately. |

## Inputs / Outputs
- **Inputs:**
  - `input` (`PinType::Data`, optional): Table containing `/address` and arguments to transmit.
- **Outputs:**
  - `output` (`PinType::Data`): Pass-through table or transmission log.
