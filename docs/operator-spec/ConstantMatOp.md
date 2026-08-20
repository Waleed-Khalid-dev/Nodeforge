# ConstantMatOp

## Overview
`ConstantMatOp` defines an unlit flat-color or wireframe material that renders independent of scene lighting, with optional texture map modulation and vertex color blending.

## Category
- **Family:** `MatOp` (MAT)
- **C++ Class:** `nf::ConstantMatOp`
- **Output Type:** `PinType::Mat` (`MaterialData`)

## Parameters

| Parameter | Type | Default | Min | Max | Description |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `color` | `Vec4` | `(1, 1, 1, 1)` | `0.0` | `1.0` | Base unlit RGBA color. |
| `wireframe` | `Bool` | `false` | - | - | Render triangle edges as wireframe lines. |
| `use_vertex_color` | `Bool` | `true` | - | - | Multiply color by per-vertex color attribute. |

## Inputs / Outputs
- **Inputs:**
  - `color_map` (`PinType::Tex`, optional): Diffuse texture map.
- **Outputs:**
  - `output` (`PinType::Mat`): Material descriptor.
