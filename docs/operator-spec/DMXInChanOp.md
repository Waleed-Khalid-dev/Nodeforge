# DMXInChanOp

## Overview
`DMXInChanOp` receives Art-Net 4 (ArtDmx) UDP packets over standard lighting port 6454 and decodes 512 channels for a specified SubNet/Universe into a real-time `ChannelBuffer`.

## Category
- **Family:** `ChanOp` (CHOP)
- **C++ Class:** `nf::DMXInChanOp`
- **Output Type:** `PinType::Channel` (`ChannelBuffer`)

## Parameters

| Parameter | Type | Default | Min | Max | Description |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `port` | `Int` | `6454` | 1024 | 65535 | Local UDP port (default 6454 for Art-Net). |
| `universe` | `Int` | `0` | 0 | 32767 | Target Art-Net SubNet/Universe index (0-32767). |
| `num_channels` | `Int` | `512` | 1 | 512 | Number of channels to extract (1..512). |
| `normalized` | `Bool` | `true` | - | - | Normalize DMX byte values [0..255] to [0.0..1.0]. |
| `active` | `Bool` | `true` | - | - | Enable/disable Art-Net receiver. |

## Outputs
- `output` (`PinType::Channel`): Channel buffer with 512 channels (`ch1`, `ch2`, ... `ch512`).
