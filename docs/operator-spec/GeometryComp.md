# GeometryComp

## Overview
`GeometryComp` is the 3D scene node that combines an input `GeometryData` mesh, a `MaterialData` shader, world transform parameters, and optional `ChannelBuffer`/`DataTable` hardware instancing.

## Category
- **Family:** `Comp` (COMP)
- **C++ Class:** `nf::GeometryComp`
- **Output Type:** `PinType::Comp` (`SceneObjectData`)

## Parameters

| Parameter | Type | Default | Min | Max | Description |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `translate` | `Vec3` | `(0, 0, 0)` | - | - | World translation (tx, ty, tz). |
| `rotate` | `Vec3` | `(0, 0, 0)` | - | - | World Euler rotation (rx, ry, rz). |
| `scale` | `Vec3` | `(1, 1, 1)` | - | - | World scale (sx, sy, sz). |
| `enable_instancing` | `Bool` | `false` | - | - | Enable GPU hardware instancing. |

## Inputs / Outputs
- **Inputs:**
  - `geometry` (`PinType::Geom`): Source 3D mesh.
  - `material` (`PinType::Mat`, optional): Shading material.
  - `instances` (`PinType::Chan` or `PinType::Data`, optional): Instancing transforms and colors.
- **Outputs:**
  - `output` (`PinType::Comp`): Scene object descriptor.
