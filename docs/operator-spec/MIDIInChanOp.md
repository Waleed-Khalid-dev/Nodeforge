# MIDIInChanOp

## Overview
`MIDIInChanOp` captures MIDI messages from hardware or virtual MIDI input ports (Notes, Control Change, Pitch Bend, Aftertouch) and outputs real-time channel values and pulse triggers.

## Category
- **Family:** `ChanOp` (CHOP)
- **C++ Class:** `nf::MIDIInChanOp`
- **Output Type:** `PinType::Channel` (`ChannelBuffer`)

## Parameters

| Parameter | Type | Default | Min | Max | Description |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `device_index` | `Int` | `0` | 0 | 32 | Hardware MIDI input device index. |
| `device_name` | `String` | `""` | - | - | Optional device name match filter. |
| `channel_filter` | `Int` | `0` | 0 | 16 | MIDI channel filter (0 = All Channels, 1..16 = Specific Channel). |
| `normalized` | `Bool` | `true` | - | - | Normalize CC and Note values to [0.0, 1.0] (false = raw 0..127). |
| `include_notes` | `Bool` | `true` | - | - | Output note channels (`ch{c}_n{note}`). |
| `include_cc` | `Bool` | `true` | - | - | Output CC channels (`ch{c}_cc{cc}`). |
| `include_pitchbend` | `Bool` | `true` | - | - | Output pitch bend channels (`ch{c}_pitch`). |
| `active` | `Bool` | `true` | - | - | Enable/disable MIDI listener. |

## Outputs
- `output` (`PinType::Channel`): Single-sample or time-sliced `ChannelBuffer` containing active MIDI channels.
