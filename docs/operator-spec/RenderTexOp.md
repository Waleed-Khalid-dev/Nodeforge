# RenderTexOp

## Overview
`RenderTexOp` renders a 3D scene composed of `GeometryComp` objects, a `CameraComp`, and `LightComp` sources into a high-resolution 2D Vulkan `gpu::Texture2D` using dynamic rendering, depth-stencil buffering, and Blinn-Phong/PBR shading.

## Category
- **Family:** `TexOp` (TOP)
- **C++ Class:** `nf::RenderTexOp`
- **Output Type:** `PinType::Tex` (`std::shared_ptr<gpu::Texture2D>`)

## Parameters

| Parameter | Type | Default | Min | Max | Description |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `resolution` | `Vec2` | `(1920, 1080)`| `16` | `8192` | Output render target resolution. |
| `clear_color`| `Vec4` | `(0.05, 0.05, 0.08, 1)` | `0.0` | `1.0` | Background clear color. |
| `enable_depth`| `Bool` | `true` | - | - | Enable `D32_SFLOAT` depth testing and writing. |
| `cull_mode` | `Int` | `1` | `0` | `2` | 0: None, 1: Back, 2: Front. |

## Inputs / Outputs
- **Inputs:**
  - `geo1` (`PinType::Comp` or `PinType::Geom`): First scene geometry / geometry component.
  - `geo2` (`PinType::Comp` or `PinType::Geom`, optional): Second scene geometry.
  - `camera` (`PinType::Comp`, optional): Active camera.
  - `light` (`PinType::Comp`, optional): Active light source.
- **Outputs:**
  - `output` (`PinType::Tex`): 3D rendered Vulkan 2D texture.
