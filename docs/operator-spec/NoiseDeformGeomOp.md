# NoiseDeformGeomOp

## Overview
`NoiseDeformGeomOp` displaces vertex positions along surface normals or world axes using 3D Simplex/Perlin gradient noise functions.

## Category
- **Family:** `GeomOp` (SOP)
- **C++ Class:** `nf::NoiseDeformGeomOp`
- **Output Type:** `PinType::Geom` (`GeometryData`)

## Parameters

| Parameter | Type | Default | Min | Max | Description |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `amplitude` | `Float` | `0.2` | `0.0` | `100.0` | Displacement strength. |
| `frequency` | `Float` | `1.0` | `0.01` | `100.0` | Spatial frequency of noise. |
| `offset` | `Vec3` | `(0, 0, 0)` | - | - | Spatial offset / animation seed. |
| `along_normal` | `Bool` | `true` | - | - | Displace along vertex normals vs world vector. |

## Inputs / Outputs
- **Inputs:**
  - `input` (`PinType::Geom`): Upstream input mesh.
- **Outputs:**
  - `output` (`PinType::Geom`): Deformed `GeometryData`.
