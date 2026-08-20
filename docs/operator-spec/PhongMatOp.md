# PhongMatOp

## Overview
`PhongMatOp` implements the classic Blinn-Phong lighting model with ambient, diffuse, specular highlights, shininess exponent, emissive glow, and multi-light accumulation.

## Category
- **Family:** `MatOp` (MAT)
- **C++ Class:** `nf::PhongMatOp`
- **Output Type:** `PinType::Mat` (`MaterialData`)

## Parameters

| Parameter | Type | Default | Min | Max | Description |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `diffuse_color` | `Vec4` | `(0.8, 0.8, 0.8, 1)` | `0.0` | `1.0` | Diffuse reflectivity. |
| `specular_color`| `Vec4` | `(1.0, 1.0, 1.0, 1)` | `0.0` | `1.0` | Specular highlight color. |
| `shininess` | `Float` | `32.0` | `1.0` | `256.0` | Specular highlight tightness. |
| `ambient_color` | `Vec4` | `(0.1, 0.1, 0.1, 1)` | `0.0` | `1.0` | Ambient light response. |
| `emissive_color`| `Vec4` | `(0.0, 0.0, 0.0, 1)` | `0.0` | `1.0` | Emissive glow color. |

## Inputs / Outputs
- **Inputs:**
  - `diffuse_map` (`PinType::Tex`, optional): Diffuse texture.
  - `normal_map` (`PinType::Tex`, optional): Tangent-space normal map.
- **Outputs:**
  - `output` (`PinType::Mat`): Material descriptor.
