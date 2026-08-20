# TransformGeomOp

## Overview
`TransformGeomOp` applies 3D translation, Euler rotation, non-uniform scaling, and pivot offsets to an upstream `GeometryData` mesh.

## Category
- **Family:** `GeomOp` (SOP)
- **C++ Class:** `nf::TransformGeomOp`
- **Output Type:** `PinType::Geom` (`GeometryData`)

## Parameters

| Parameter | Type | Default | Min | Max | Description |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `translate` | `Vec3` | `(0, 0, 0)` | - | - | Translation vector (tx, ty, tz). |
| `rotate` | `Vec3` | `(0, 0, 0)` | - | - | Rotation angles in degrees (rx, ry, rz). |
| `scale` | `Vec3` | `(1, 1, 1)` | - | - | Scaling factors (sx, sy, sz). |
| `pivot` | `Vec3` | `(0, 0, 0)` | - | - | Center of rotation and scaling. |

## Inputs / Outputs
- **Inputs:**
  - `input` (`PinType::Geom`): Upstream input mesh.
- **Outputs:**
  - `output` (`PinType::Geom`): Transformed `GeometryData`.
