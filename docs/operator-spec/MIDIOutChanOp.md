# MIDIOutChanOp

## Overview
`MIDIOutChanOp` converts incoming control channels into MIDI messages (Notes, Control Change, Pitch Bend, Program Change) transmitted to hardware synths or virtual MIDI output ports.

## Category
- **Family:** `ChanOp` (CHOP)
- **C++ Class:** `nf::MIDIOutChanOp`
- **Output Type:** `PinType::Channel` (`ChannelBuffer` passthrough)

## Parameters

| Parameter | Type | Default | Min | Max | Description |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `device_index` | `Int` | `0` | 0 | 32 | Hardware MIDI output device index. |
| `midi_channel` | `Int` | `1` | 1 | 16 | Target MIDI channel (1-16). |
| `mode` | `Int` | `0` | 0 | 2 | Mode: 0=Auto-Detect from Names, 1=Continuous CC, 2=Note Triggers. |
| `cc_number` | `Int` | `1` | 0 | 127 | Default CC number for single channel mode. |
| `threshold` | `Float` | `0.5` | 0.0 | 1.0 | Note On trigger threshold. |
| `active` | `Bool` | `true` | - | - | Enable/disable MIDI transmission. |

## Inputs / Outputs
- **Inputs:**
  - `input` (`PinType::Channel`): Channel values to transmit as MIDI events.
- **Outputs:**
  - `output` (`PinType::Channel`): Passthrough channel buffer.
