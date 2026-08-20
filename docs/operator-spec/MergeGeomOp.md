# MergeGeomOp

## Overview
`MergeGeomOp` combines two incoming `GeometryData` mesh streams into a single consolidated mesh, concatenating vertices and re-indexing triangle faces with zero data loss.

## Category
- **Family:** `GeomOp` (SOP)
- **C++ Class:** `nf::MergeGeomOp`
- **Output Type:** `PinType::Geom` (`GeometryData`)

## Parameters

| Parameter | Type | Default | Min | Max | Description |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `recompute_normals` | `Bool` | `false` | - | - | Automatically recalculate combined normals. |

## Inputs / Outputs
- **Inputs:**
  - `input1` (`PinType::Geom`): First input mesh.
  - `input2` (`PinType::Geom`): Second input mesh.
- **Outputs:**
  - `output` (`PinType::Geom`): Combined `GeometryData`.
