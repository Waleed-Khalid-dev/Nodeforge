# ChanToGeomOp

## Overview
`ChanToGeomOp` (ChopToSop) creates 3D point clouds, connected particle line strips, or instanced geometric point distributions from an input `ChannelBuffer` (e.g. `tx, ty, tz` or `r, g, b` tracks).

## Category
- **Family:** `GeomOp` (SOP)
- **C++ Class:** `nf::ChanToGeomOp`
- **Output Type:** `PinType::Geom` (`GeometryData`)

## Parameters

| Parameter | Type | Default | Min | Max | Description |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `chan_x` | `String` | `"tx"` | - | - | Channel name for X coordinates. |
| `chan_y` | `String` | `"ty"` | - | - | Channel name for Y coordinates. |
| `chan_z` | `String` | `"tz"` | - | - | Channel name for Z coordinates. |
| `primitive_type` | `Int` | `0` | `0` | `2` | 0: Point Cloud / Sprites, 1: Line Strip, 2: Triangles. |

## Inputs / Outputs
- **Inputs:**
  - `input` (`PinType::Chan`): Source `ChannelBuffer`.
- **Outputs:**
  - `output` (`PinType::Geom`): Generated 3D point/line `GeometryData`.
