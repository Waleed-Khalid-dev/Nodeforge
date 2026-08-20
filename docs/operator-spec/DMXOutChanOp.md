# DMXOutChanOp

## Overview
`DMXOutChanOp` converts input channels into 512-channel DMX frames packaged as Art-Net 4 (`ArtDmx`) UDP packets broadcast or unicast to lighting nodes, dimmers, and LED fixtures.

## Category
- **Family:** `ChanOp` (CHOP)
- **C++ Class:** `nf::DMXOutChanOp`
- **Output Type:** `PinType::Channel` (`ChannelBuffer` passthrough)

## Parameters

| Parameter | Type | Default | Min | Max | Description |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `host` | `String` | `"255.255.255.255"` | - | - | Destination IP address (Broadcast or Unicast). |
| `port` | `Int` | `6454` | 1024 | 65535 | Destination UDP port (default 6454). |
| `universe` | `Int` | `0` | 0 | 32767 | Destination Art-Net SubNet/Universe (0-32767). |
| `rate_hz` | `Float` | `44.0` | 1.0 | 60.0 | Maximum transmission packet rate (DMX standard 44Hz). |
| `input_normalized`| `Bool` | `true` | - | - | Input channels are normalized [0.0..1.0] (false = raw 0..255). |
| `active` | `Bool` | `true` | - | - | Enable/disable Art-Net transmission. |

## Inputs / Outputs
- **Inputs:**
  - `input` (`PinType::Channel`): Channel buffer with lighting channel values (up to 512 channels).
- **Outputs:**
  - `output` (`PinType::Channel`): Passthrough channel buffer.
