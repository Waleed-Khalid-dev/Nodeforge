# Neo Realms — Holographic Interactive Particle Swarm

**Project:** `samples/05_holographic_particles/holographic_particles.nfp`  
**Target Hardware:** Transparent OLED / Holographic Mesh / LED Volume / Touch/LiDAR Tracking  
**Primary Engine Features:** `ParticleEmitterGeomOp`, `ParticleForceGeomOp`, `ParticleAttractorGeomOp`, `ParticleMatOp`, `OSCInChanOp`, `GeometryComp`, `RenderTexOp`, `BlurTexOp`, `CompositeTexOp`.

---

## 1. Overview

This project implements a real-time holographic particle swarm simulation capable of simulating and rendering 100,000 to 1,000,000 dynamic particles. An incoming Open Sound Control (OSC) hand tracking stream (e.g. from Leap Motion, MediaPipe, or LiDAR) dynamically moves a 3D spherical attractor point in space, pulling and scattering organic curl turbulence fluid streams in real time.

```
       [ OSC Hand Tracking / LiDAR ]
                    │
                    ▼
          ┌───────────────────┐
          │   OSCInChanOp     │ (/hand/pos_x, pos_y, pos_z)
          └─────────┬─────────┘
                    │ (Target Coordinate)
                    │
  ┌─────────────────▼────────┐
  │  ParticleEmitterGeomOp   │ (100,000 Particles, 10,000/sec)
  └─────────┬────────────────┘
            ▼
  ┌──────────────────────────┐
  │   ParticleForceGeomOp    │ (3D Simplex Curl Turbulence + Drag)
  └─────────┬────────────────┘
            ▼
  ┌──────────────────────────┐
  │ ParticleAttractorGeomOp  │ (Dynamic Gesture Attractor)
  └─────────┬────────────────┘
            │
            ├──────────────────────┐
            ▼                      ▼
     ┌──────────────┐       ┌──────────────┐
     │ GeometryComp │◄──────│ParticleMatOp │ (Additive Blend)
     └──────┬───────┘       └──────────────┘
            │
            ├─────────────── [CameraComp]
            ├─────────────── [LightComp]
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
            │    ToWindowTexOp     │ (60 FPS Holographic Display)
            └──────────────────────┘
```

---

## 2. Real-Time Interaction (OSC Stream)

The network listens on UDP Port `8000` for `/hand/*` coordinates:

| Address | Type | Description |
|---|---|---|
| `/hand/x` | Float | World X position of user hand `[-5.0 .. 5.0]` |
| `/hand/y` | Float | World Y position of user hand `[-3.0 .. 6.0]` |
| `/hand/z` | Float | World Z position of user hand `[-5.0 .. 5.0]` |
