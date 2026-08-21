# Neo Realms — Live Laser Concert & Kinetic Beam Symphony

**Project:** `samples/12_laser_symphony_spectacular/laser_symphony_spectacular.nfp`  
**Target Hardware:** Ether Dream (Ethernet) / Helios (USB) / Standard ILDA Laser Projectors  
**Primary Engine Features:** `LaserPatternGeomOp`, `LaserGeomOp`, `LaserDACChanOp`, `VoxelGridGeomOp`, `VolumetricFogMatOp`, `RenderTexOp`, `LightShaftTexOp`, `AudioSpatializerChanOp`.

---

## 1. Overview

This project demonstrates real-time **Galvanometer Laser Scanning, ILDA Vector Formatting, and Hardware DAC Output** inside NodeForge.

```
┌────────────────────────┐
│   LaserPatternGeomOp   │ (Lissajous & Spirograph Generator)
└───────────┬────────────┘
            │
            ▼
┌────────────────────────┐
│      LaserGeomOp       │ (Galvo Path Optimizer: Blanking & Dwells)
└─────┬────────────┬─────┘
      │            │
      │ (Points)   │ (Geom)
      ▼            ▼
┌───────────┐ ┌────────────────────────┐
│LaserDAC.. │ │      GeometryComp      │ ──► Scene3DPass (3D Smoke) ──► LightShaftTexOp
└───────────┘ └────────────────────────┘
```

---

## 2. Interactive Features

- **Procedural Lissajous & Spirographs:** Real-time geometric laser patterns with harmonic ratios.
- **Galvo Path Optimization:** Automatic blanking point insertion and corner dwells to protect scanner mirrors.
- **Hardware DAC Streaming:** 30 kpps point streaming with safety shutter control.
- **3D Volumetric Smoke Illumination:** Beams project through atmospheric smoke zones.
