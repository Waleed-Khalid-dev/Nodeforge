# KeyboardInChanOp

## Overview
`KeyboardInChanOp` tracks keyboard key states, momentary pulse triggers, and modifier keys (`ctrl`, `shift`, `alt`, `super`), enabling interactive stage hotkeys and kiosk user interaction.

## Category
- **Family:** `ChanOp` (CHOP)
- **C++ Class:** `nf::KeyboardInChanOp`
- **Output Type:** `PinType::Channel` (`ChannelBuffer`)

## Parameters

| Parameter | Type | Default | Min | Max | Description |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `keys` | `String` | `"space enter escape left right up down w a s d 1 2 3"` | - | - | Space-separated list of keys to monitor. |
| `include_modifiers`| `Bool` | `true` | - | - | Include modifier channels (`ctrl`, `shift`, `alt`, `meta`). |
| `include_pulses` | `Bool` | `true` | - | - | Include single-frame pulse channels (`pulse_{key}`). |
| `global_capture` | `Bool` | `true` | - | - | Capture keys globally across desktop (via Win32). |
| `active` | `Bool` | `true` | - | - | Enable/disable keyboard tracking. |

## Channels Generated
- `key_{name}`: 1.0 when held down, 0.0 when released
- `pulse_{name}`: 1.0 for the exact frame key is pressed, 0.0 otherwise
- `ctrl`, `shift`, `alt`, `meta`: Modifier state values (1.0 / 0.0)
