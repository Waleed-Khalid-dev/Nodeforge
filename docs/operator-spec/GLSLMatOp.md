# GLSLMatOp

## Overview
`GLSLMatOp` compiles and executes custom GLSL vertex and fragment shaders at runtime with live hot-reloading and uniform bindings for real-time visual exploration.

## Category
- **Family:** `MatOp` (MAT)
- **C++ Class:** `nf::GLSLMatOp`
- **Output Type:** `PinType::Mat` (`MaterialData`)

## Parameters

| Parameter | Type | Default | Min | Max | Description |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `vertex_shader` | `String` | `""` | - | - | Custom GLSL vertex shader code. |
| `fragment_shader`| `String` | `""` | - | - | Custom GLSL fragment shader code. |
| `wireframe` | `Bool` | `false` | - | - | Wireframe rasterization mode. |

## Inputs / Outputs
- **Inputs:**
  - `map1` (`PinType::Tex`, optional): Texture sampler 0.
  - `map2` (`PinType::Tex`, optional): Texture sampler 1.
- **Outputs:**
  - `output` (`PinType::Mat`): Compiled GLSL material pipeline.
