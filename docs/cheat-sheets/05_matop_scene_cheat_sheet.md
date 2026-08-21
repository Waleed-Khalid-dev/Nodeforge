# MatOp & Scene Comp Cheat Sheet (Materials, Shaders, Cameras & Lights)

**Family:** `MatOp` / `Comp` | **Color:** Gold (`#FFD600`) | **Pipeline:** Vulkan 1.3 3D Rasterization Pipeline

---

## 1. Material & 3D Scene Architecture

Materials define how light interacts with 3D geometry. Scene Components (`GeometryComp`, `CameraComp`, `LightComp`) assemble meshes, materials, and viewpoints into a renderable 3D scene fed into `RenderTexOp`.

---

## 2. Core Operator Reference (6 Operators)

| Operator | In Pins | Out Pins | Key Parameters | Cook Behavior |
|----------|---------|----------|----------------|---------------|
| **`ConstantMatOp`** | None | `output` (Mat) | `color` (vec4), `wireframe` (bool) | Unlit flat color material |
| **`PhongMatOp`** | None | `output` (Mat) | `ambient`, `diffuse`, `specular`, `shininess`, `diffuse_texture` | Classic Blinn-Phong lighting model with specular highlights |
| **`GLSLMatOp`** | None | `output` (Mat) | `vertex_source` (string), `fragment_source` (string), `push_constants` | Custom programmable Vulkan SPIR-V shader material |
| **`CameraComp`** | None | `output` (Camera) | `fov` (float), `near_plane`, `far_plane`, `position` (vec3), `target` (vec3) | Defines perspective view and projection matrix |
| **`LightComp`** | None | `output` (Light) | `color` (vec3), `intensity` (float), `position` (vec3), `type` (Point/Directional) | Defines world lighting source |
| **`GeometryComp`** | `geometry`, `material` | `output` (GeoComp) | `enable_instancing` (bool), `instance_source` (ChanOp/DataOp) | Binds mesh data to material for GPU rasterization |

---

## 3. RenderTexOp Scene Assembly

```
  [Mesh: GeomOp] ──► [GeometryComp] ──┐
  [Mat: MatOp]   ──►                  ├──► [RenderTexOp] ──► [2D Texture Stream]
  [CameraComp]   ─────────────────────┤
  [LightComp]    ─────────────────────┘
```
