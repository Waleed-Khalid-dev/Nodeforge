# Neo Realms — Interactive Volumetric Nebula & Cosmic Cathedral

**Project:** `samples/11_volumetric_nebula_cathedral/volumetric_nebula_cathedral.nfp`  
**Primary Engine Features:** `VolumetricCloudTexOp`, `VolumetricFogMatOp`, `VoxelGridGeomOp`, `LightShaftTexOp`, `RenderTexOp`, `AudioSpatializerChanOp`.

---

## 1. Overview

This project demonstrates real-time **Volumetric Atmospheric Ray Marching, 3D Smoke Plumes, and Crepuscular God-Ray Light Shafts** inside NodeForge.

```
┌────────────────────────┐
│  VolumetricCloudTexOp  │ (Procedural Sunset Clouds & Forward Scattering)
└───────────┬────────────┘
            │
            ├──────────────────────────────────────────┐
            ▼                                          ▼
┌────────────────────────┐                 ┌────────────────────────┐
│   RenderTexOp (Scene)  │ ──────────────► │    LightShaftTexOp     │ (God-Ray Beams)
└────────────────────────┘                 └────────────────────────┘
```

---

## 2. Interactive Features

- **Procedural 3D Atmospheric Clouds:** Continuous density accumulation, Beer-Lambert extinction, and Henyey-Greenstein silver-lining forward scattering.
- **Volumetric God-Rays:** Screen-space radial blur light shafts piercing architectural arches.
- **3D Localized Smoke Material:** `VolumetricFogMatOp` for participating medium shaders.
- **Spatial Audio Synergy:** Dynamic sound spatialization tracking light source angles.
