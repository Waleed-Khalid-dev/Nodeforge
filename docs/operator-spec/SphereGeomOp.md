# SphereGeomOp

## Overview
`SphereGeomOp` generates a 3D UV sphere mesh with parameterized radius, latitudinal rings, and longitudinal segments.

## Category
- **Family:** `GeomOp` (SOP)
- **C++ Class:** `nf::SphereGeomOp`
- **Output Type:** `PinType::Geom` (`GeometryData`)

## Parameters

| Parameter | Type | Default | Min | Max | Description |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `radius` | `Float` | `1.0` | `0.001` | `1000.0` | Radius of the sphere. |
| `rings` | `Int` | `16` | `3` | `512` | Number of latitudinal rings. |
| `segments` | `Int` | `32` | `3` | `512` | Number of longitudinal segments. |

## Inputs / Outputs
- **Inputs:** None (Generator).
- **Outputs:**
  - `output` (`PinType::Geom`): Generated sphere `GeometryData`.
