# Neo Realms — Flagship Façade Mapping Show

**Project:** `samples/01_facade_mapping/facade_mapping.nfp`  
**Target Hardware:** Dual 1080p/4K Laser Projectors (RTX 3060+ GPU)  
**Primary Engine Features:** `WarpBlendTexOp`, `ProjectorOutTexOp`, `NoiseTexOp`, `CompositeTexOp`, `BlurTexOp`, `LFOChanOp`.

---

## 1. Overview

This flagship project represents Neo Realms' primary commercial installation workflow: driving a dual-projector architectural façade mapping setup with real-time generative GPU graphics, independent 2D Bezier grid warping, S-curve softedge blending, and multi-display physical presentation windows.

```
                  ┌──────────────┐
                  │ TimeChanOp   │
                  └──────┬───────┘
                         │
                  ┌──────▼───────┐
                  │  LFOChanOp   │
                  └──────┬───────┘
                         │ (Expression Binding)
                  ┌──────▼───────┐
                  │  NoiseTexOp  │
                  └──┬────────┬──┘
                     │        │
           ┌─────────▼──┐  ┌──▼──────────┐
           │ BlurTexOp  │  │ (Direct)    │
           └─────────┬──┘  └──┬──────────┘
                     │        │
                  ┌──▼────────▼──┐
                  │CompositeTexOp│ (Add Blend)
                  └──┬────────┬──┘
                     │        │
     ┌───────────────▼──┐  ┌──▼───────────────┐
     │WarpBlend (Left)  │  │WarpBlend (Right) │
     └───────┬──────────┘  └───────┬──────────┘
             │                     │
     ┌───────▼──────────┐  ┌───────▼──────────┐
     │ ProjectorOut 1   │  │ ProjectorOut 2   │
     │ (Display Index 1)│  │ (Display Index 2)│
     └──────────────────┘  └──────────────────┘
```

---

## 2. Signal & Graph Architecture

1. **Master Clock & LFO (`TimeChanOp` -> `LFOChanOp`):** Generates smooth, continuous phase signals at 60 FPS.
2. **Generative GPU Shader (`NoiseTexOp`):** Multi-octave Perlin noise pattern modulated dynamically via the expression `op('anim_lfo')['chan1'] * 5.0`.
3. **Glow Composite (`BlurTexOp` + `CompositeTexOp`):** Generates an ethereal bloom glow by adding a multi-pass separable Gaussian blur pass back over the original texture.
4. **Warp & Softedge Engine (`WarpBlendTexOp`):**
   - **Left Projector:** `blend_edge = 1` (Right edge overlap), `blend_width = 0.15` (15% overlap area), `blend_gamma = 2.2`.
   - **Right Projector:** `blend_edge = 0` (Left edge overlap), `blend_width = 0.15`, `blend_gamma = 2.2`.
5. **Physical Output Routing (`ProjectorOutTexOp`):** Spawns borderless, hardware-synchronized presentation windows on OS monitors 1 and 2.

---

## 3. On-Site Calibration Procedure

1. **Launch Project:** Open `facade_mapping.nfp` in NodeForge Studio or run standalone via:
   ```cmd
   nodeforge_player.exe --project samples/01_facade_mapping/facade_mapping.nfp --fullscreen
   ```
2. **Enable Calibration Grid:** In the Parameter Inspector for `warp_blend_left` and `warp_blend_right`, set `show_grid = true`.
3. **Interactive Warping:** In the Viewer panel, select the control points on the 4x4 Bezier mesh to align with physical architectural pillars and window recesses.
4. **Tune Edge Blending:** Adjust `blend_width` and `blend_gamma` until the projector seam disappears seamlessly under ambient lighting.
5. **Save Configuration:** Press `Ctrl + S` to save the active warp coordinates into the `.nfp` project.
