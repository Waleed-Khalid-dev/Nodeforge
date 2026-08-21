# TexOp Family Cheat Sheet (2D GPU Textures & Media I/O)

**Family:** `TexOp` | **Color:** Cyan (`#00E5FF`) | **API:** Vulkan 1.3 Dynamic Rendering

---

## 1. TexOp Architecture

All TexOps operate asynchronously on the GPU. Textures are managed via the zero-allocation `TexturePool` using leased `VkImage` / `VkImageView` descriptors.

---

## 2. Core Operator Reference (16 Operators)

| Operator | In Pins | Out Pins | Key Parameters | Cook Behavior |
|----------|---------|----------|----------------|---------------|
| **`NullTexOp`** | `input` (Tex) | `output` (Tex) | `bypass` (bool) | Zero-copy pass-through alias |
| **`ConstantTexOp`** | None | `output` (Tex) | `color` (vec4), `width` (int), `height` (int) | Clears texture to solid color |
| **`NoiseTexOp`** | None | `texture` (Tex) | `type` (Perlin/Simplex/Worley), `frequency` (float), `time` (float), `harmonics` (int) | Procedural GPU noise shader |
| **`LoadImageTexOp`** | None | `output` (Tex) | `file_path` (string), `reload` (pulse) | Decodes PNG/JPG/EXR to GPU |
| **`TransformTexOp`** | `input` (Tex) | `output` (Tex) | `translate` (vec2), `rotate` (float), `scale` (vec2), `tile_mode` (int) | 2D affine UV matrix transformation |
| **`CompositeTexOp`** | `input_a`, `input_b` | `output` (Tex) | `operation` (Over, Add, Mul, Screen, Sub), `opacity` (float) | Dual-texture blending pass |
| **`BlurTexOp`** | `input` (Tex) | `output` (Tex) | `radius` (int), `sigma` (float), `passes` (int) | Separable Gaussian 2-pass blur |
| **`LevelTexOp`** | `input` (Tex) | `output` (Tex) | `brightness`, `contrast`, `gamma`, `black_level`, `white_level` | Real-time color correction & grading |
| **`ResolutionTexOp`** | `input` (Tex) | `output` (Tex) | `width` (int), `height` (int), `filter_mode` (Bilinear/Nearest) | GPU texture resampler & scaler |
| **`ToWindowTexOp`** | `input` (Tex) | None | `title` (string), `fullscreen` (bool) | Presents texture to primary viewport |
| **`MovieFileInTexOp`** | None | `output` (Tex) | `file_path` (string), `play` (bool), `speed` (float), `loop` (bool) | Multi-threaded FFmpeg video player |
| **`VideoDeviceInTexOp`**| None | `output` (Tex) | `device_index` (int), `format` (int) | USB webcam / capture card live video |
| **`SpoutInTexOp`** | None | `output` (Tex) | `sender_name` (string) | Zero-copy Windows GPU texture receiver |
| **`SpoutOutTexOp`** | `input` (Tex) | None | `sender_name` (string) | Zero-copy Windows GPU texture broadcaster |
| **`NDIInTexOp`** | None | `output` (Tex) | `source_name` (string) | Low-latency IP video network stream in |
| **`NDIOutTexOp`** | `input` (Tex) | None | `stream_name` (string) | Low-latency IP video network stream out |
| **`ProjectorOutTexOp`** | `input` (Tex) | None | `display_index` (int), `fullscreen` (bool), `borderless` (bool) | Hardware-addressed physical display window |
| **`WarpBlendTexOp`** | `input` (Tex) | `output` (Tex) | `screen_index`, `grid_rows`, `grid_cols`, `blend_edge`, `blend_width`, `blend_gamma` | 2D Bezier warping & softedge blending |
| **`RenderTexOp`** | `geometry`, `camera`, `light` | `output` (Tex) | `width` (int), `height` (int), `clear_color` (vec4) | 3D scene rasterizer to 2D texture |

---

## 3. Common Idioms

```
# Glow / Bloom Filter
SourceTexOp ──┬──► BlurTexOp (Radius: 16) ──► CompositeTexOp (Mode: Add) ──► LevelTexOp
              └─────────────────────────────┘

# Dual-Screen Projection Façade Mapping
MainRender ──┬──► WarpBlendTexOp (Screen 0, Right Edge Blend) ──► ProjectorOut (Display 1)
             └──► WarpBlendTexOp (Screen 1, Left Edge Blend)  ──► ProjectorOut (Display 2)
```
