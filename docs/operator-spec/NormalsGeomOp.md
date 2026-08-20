# NormalsGeomOp

## Overview
`NormalsGeomOp` calculates, smooths, or inverts vertex normals and calculates tangent/bitangent vectors for normal mapping.

## Category
- **Family:** `GeomOp` (SOP)
- **C++ Class:** `nf::NormalsGeomOp`
- **Output Type:** `PinType::Geom` (`GeometryData`)

## Parameters

| Parameter | Type | Default | Min | Max | Description |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `mode` | `Int` | `0` | `0` | `2` | 0: Smooth (Area Weighted), 1: Faceted/Flat, 2: Invert. |
| `compute_tangents` | `Bool` | `true` | - | - | Generate tangent vectors (`vec4`) for normal maps. |

## Inputs / Outputs
- **Inputs:**
  - `input` (`PinType::Geom`): Upstream input mesh.
- **Outputs:**
  - `output` (`PinType::Geom`): Recalculated `GeometryData`.
