# TorusGeomOp

## Overview
`TorusGeomOp` procedurally generates a 3D donut/toroid mesh with major/minor radii and resolution settings.

## Category
- **Family:** `GeomOp` (SOP)
- **C++ Class:** `nf::TorusGeomOp`
- **Output Type:** `PinType::Geom` (`GeometryData`)

## Parameters

| Parameter | Type | Default | Min | Max | Description |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `major_radius` | `Float` | `1.0` | `0.001` | `1000.0` | Radius from center to tube midpoint. |
| `minor_radius` | `Float` | `0.3` | `0.001` | `1000.0` | Radius of the tube itself. |
| `major_segments`| `Int` | `32` | `3` | `512` | Number of segments around main ring. |
| `minor_segments`| `Int` | `16` | `3` | `512` | Number of segments around tube cross-section. |

## Inputs / Outputs
- **Inputs:** None (Generator).
- **Outputs:**
  - `output` (`PinType::Geom`): Generated torus `GeometryData`.
