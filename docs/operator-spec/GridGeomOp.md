# GridGeomOp

## Overview
`GridGeomOp` procedurally generates a planar 2D grid/plane mesh in 3D space with configurable rows, columns, dimensions, and UV coordinate generation.

## Category
- **Family:** `GeomOp` (SOP)
- **C++ Class:** `nf::GridGeomOp`
- **Output Type:** `PinType::Geom` (`GeometryData`)

## Parameters

| Parameter | Type | Default | Min | Max | Description |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `size_x` | `Float` | `1.0` | `0.001` | `1000.0` | Width of the grid along the X axis. |
| `size_y` | `Float` | `1.0` | `0.001` | `1000.0` | Height/depth of the grid along the Y/Z axis. |
| `rows` | `Int` | `10` | `2` | `1000` | Number of subdivisions along the vertical axis. |
| `cols` | `Int` | `10` | `2` | `1000` | Number of subdivisions along the horizontal axis. |
| `plane` | `Int` | `0` | `0` | `2` | 0: XY Plane, 1: XZ Plane, 2: YZ Plane. |

## Inputs / Outputs
- **Inputs:** None (Generator).
- **Outputs:**
  - `output` (`PinType::Geom`): Generated grid `GeometryData`.
