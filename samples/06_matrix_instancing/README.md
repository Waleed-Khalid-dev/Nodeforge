# Neo Realms — Audio-Reactive Kinetic Matrix Instancing

**Project:** `samples/06_matrix_instancing/matrix_instancing.nfp`  
**Target Hardware:** LED Wall / Immersive Dome / Stage Backdrop / Transparent Displays  
**Primary Engine Features:** `InstanceGeomOp`, `BoxGeomOp`, `AudioFileInChanOp`, `MathChanOp`, `PhongMatOp`, `GeometryComp`, `CameraComp`, `RenderTexOp`, `BlurTexOp`, `CompositeTexOp`.

---

## 1. Overview

This project demonstrates massive high-performance **GPU Instancing** in NodeForge, rendering a kinetic matrix of thousands to 100,000 crystal meshes. Real-time audio frequency bands are passed into `InstanceGeomOp`, creating cascading wave ripples, specular light glints, and dynamic color progression across the matrix array.

```
       [ Audio File / Microphone Stream ]
                        │
                        ▼
               ┌─────────────────┐
               │AudioFileInChanOp│
               └────────┬────────┘
                        │
                        ▼
               ┌─────────────────┐
               │   MathChanOp    │ (Amplitude Gain)
               └────────┬────────┘
                        │
  ┌─────────────────┐   │ (SIMD Modulation Channels)
  │    BoxGeomOp    │   │
  │ (Template Mesh) │   │
  └────────┬────────┘   │
           │            │
           ▼            ▼
  ┌──────────────────────────────┐
  │        InstanceGeomOp        │ (Grid Array [60x60] / Spiral)
  └──────────────┬───────────────┘
                 │
                 ├──────────────────────┐
                 ▼                      ▼
          ┌──────────────┐       ┌──────────────┐
          │ GeometryComp │◄──────│  PhongMatOp  │ (Blinn-Phong Specular)
          └──────┬───────┘       └──────────────┘
                 │
                 ├─────────────── [CameraComp] (Orbit View)
                 ├─────────────── [LightComp]  (Key Light)
                 ▼
          ┌──────────────┐
          │ RenderTexOp  │ (1920x1080 Dynamic Raster)
          └──────┬───────┘
                 │
                 ├──────────────────────┐
                 ▼                      ▼
          ┌──────────────┐       ┌──────────────┐
          │  BlurTexOp   │       │   (Direct)   │
          │ (Bloom Glow) │       │              │
          └──────┬───────┘       └──────┬───────┘
                 │                      │
                 └──────────┬───────────┘
                            ▼
                 ┌──────────────────────┐
                 │    CompositeTexOp    │ (Additive Glow)
                 └──────────┬───────────┘
                            ▼
                 ┌──────────────────────┐
                 │    ToWindowTexOp     │ (60 FPS Window)
                 └──────────────────────┘
```

---

## 2. Instancing Performance

- **Draw Call Efficiency:** Single instanced draw call rendering 3,600 to 100,000 objects.
- **VRAM Stability:** Contiguous instance buffer allocation with zero memory fragmentation.
