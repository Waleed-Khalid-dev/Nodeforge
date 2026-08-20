# MouseInChanOp

## Overview
`MouseInChanOp` captures mouse cursor position, motion deltas, button presses, and scroll wheel events from the active editor window, physical projector outputs, or the global desktop.

## Category
- **Family:** `ChanOp` (CHOP)
- **C++ Class:** `nf::MouseInChanOp`
- **Output Type:** `PinType::Channel` (`ChannelBuffer`)

## Parameters

| Parameter | Type | Default | Min | Max | Description |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `scope` | `Int` | `0` | 0 | 2 | Scope: 0=Focused Window, 1=Projector Output, 2=Global Desktop. |
| `normalized` | `Bool` | `true` | - | - | Output coordinates as [0.0..1.0] (false = raw pixels). |
| `active` | `Bool` | `true` | - | - | Enable/disable mouse capture. |

## Channels Generated
- `x`: Horizontal position (normalized [0..1] or pixels)
- `y`: Vertical position (normalized [0..1] or pixels, inverted $0$ at top or bottom)
- `dx`: Frame delta X
- `dy`: Frame delta Y
- `left`: Left button state (1.0 = pressed, 0.0 = released)
- `right`: Right button state
- `middle`: Middle button state
- `wheel_x`: Horizontal scroll delta
- `wheel_y`: Vertical scroll delta
- `hover`: 1.0 if inside window bounds, 0.0 otherwise
