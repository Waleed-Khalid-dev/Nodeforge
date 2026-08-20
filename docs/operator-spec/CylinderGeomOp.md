# CylinderGeomOp

## Overview
`CylinderGeomOp` generates a 3D cylindrical tube, cone, or truncated cone mesh with configurable top/bottom radii, height, and caps.

## Category
- **Family:** `GeomOp` (SOP)
- **C++ Class:** `nf::CylinderGeomOp`
- **Output Type:** `PinType::Geom` (`GeometryData`)

## Parameters

| Parameter | Type | Default | Min | Max | Description |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `height` | `Float` | `2.0` | `0.001` | `1000.0` | Total height along Y axis. |
| `radius_bottom`| `Float` | `1.0` | `0.0` | `1000.0` | Base radius. |
| `radius_top` | `Float` | `1.0` | `0.0` | `1000.0` | Top cap radius (0 for cone). |
| `segments` | `Int` | `32` | `3` | `512` | Number of radial segments. |
| `cap_bottom` | `Bool` | `true` | - | - | Close base cap. |
| `cap_top` | `Bool` | `true` | - | - | Close top cap. |

## Inputs / Outputs
- **Inputs:** None (Generator).
- **Outputs:**
  - `output` (`PinType::Geom`): Generated cylinder `GeometryData`.
