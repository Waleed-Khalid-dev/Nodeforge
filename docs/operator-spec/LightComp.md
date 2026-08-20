# LightComp

## Overview
`LightComp` creates a light source within the 3D scene graph, supporting Directional, Point, and Ambient light models with color, intensity, and attenuation.

## Category
- **Family:** `Comp` (COMP)
- **C++ Class:** `nf::LightComp`
- **Output Type:** `PinType::Comp` (`LightData`)

## Parameters

| Parameter | Type | Default | Min | Max | Description |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `light_type` | `Int` | `0` | `0` | `2` | 0: Directional, 1: Point, 2: Ambient. |
| `color` | `Vec3` | `(1, 1, 1)` | `0.0` | `10.0` | Light color RGB. |
| `intensity` | `Float` | `1.0` | `0.0` | `100.0` | Brightness multiplier. |
| `position` | `Vec3` | `(5, 5, 5)` | - | - | Position (Point Light). |
| `direction` | `Vec3` | `(-1, -1, -1)`| - | - | Direction vector (Directional Light). |
| `attenuation`| `Float` | `0.05` | `0.0` | `10.0` | Distance attenuation factor. |

## Inputs / Outputs
- **Inputs:** None.
- **Outputs:**
  - `output` (`PinType::Comp`): Light descriptor.
