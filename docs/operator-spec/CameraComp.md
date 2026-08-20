# CameraComp

## Overview
`CameraComp` defines the virtual 3D camera in the scene, computing View and Projection matrices with support for Perspective (FOV, Aspect, Near, Far) and Orthographic modes.

## Category
- **Family:** `Comp` (COMP)
- **C++ Class:** `nf::CameraComp`
- **Output Type:** `PinType::Comp` (`CameraData`)

## Parameters

| Parameter | Type | Default | Min | Max | Description |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `projection_type` | `Int` | `0` | `0` | `1` | 0: Perspective, 1: Orthographic. |
| `fov_y` | `Float` | `45.0` | `1.0` | `170.0` | Vertical field of view in degrees. |
| `near_plane` | `Float` | `0.1` | `0.001` | `1000.0` | Near clipping plane distance. |
| `far_plane` | `Float` | `1000.0` | `1.0` | `100000.0`| Far clipping plane distance. |
| `position` | `Vec3` | `(0, 0, 5)` | - | - | Camera 3D world position. |
| `look_at` | `Vec3` | `(0, 0, 0)` | - | - | Target look-at point. |
| `up_vector` | `Vec3` | `(0, 1, 0)` | - | - | Up orientation vector. |

## Inputs / Outputs
- **Inputs:** None.
- **Outputs:**
  - `output` (`PinType::Comp`): Camera descriptor.
