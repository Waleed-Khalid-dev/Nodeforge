# BoxGeomOp

## Overview
`BoxGeomOp` procedurally generates a 3D cuboid mesh with independent dimensions, face subdivisions, and unique per-face UV mappings.

## Category
- **Family:** `GeomOp` (SOP)
- **C++ Class:** `nf::BoxGeomOp`
- **Output Type:** `PinType::Geom` (`GeometryData`)

## Parameters

| Parameter | Type | Default | Min | Max | Description |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `size_x` | `Float` | `1.0` | `0.001` | `1000.0` | Size along X axis. |
| `size_y` | `Float` | `1.0` | `0.001` | `1000.0` | Size along Y axis. |
| `size_z` | `Float` | `1.0` | `0.001` | `1000.0` | Size along Z axis. |
| `divs_x` | `Int` | `1` | `1` | `100` | Subdivisions along X. |
| `divs_y` | `Int` | `1` | `1` | `100` | Subdivisions along Y. |
| `divs_z` | `Int` | `1` | `1` | `100` | Subdivisions along Z. |

## Inputs / Outputs
- **Inputs:** None (Generator).
- **Outputs:**
  - `output` (`PinType::Geom`): Generated box `GeometryData`.
