# Neo Realms — Immersive 3D Spatial Audio & Planetarium Dome

**Project:** `samples/08_spatial_audio_dome/spatial_audio_dome.nfp`  
**Target Hardware:** Planetarium Geodesic Dome / 7.1.4 Dolby Atmos Theater / Multi-Speaker Ring  
**Primary Engine Features:** `AudioSpatializerChanOp`, `AmbisonicDecodeChanOp`, `AudioEmitterComp`, `AudioListenerComp`, `AudioFileInChanOp`, `SphereGeomOp`, `RenderTexOp`, `ToWindowTexOp`.

---

## 1. Overview

This project demonstrates real-time **3D Spatial Audio & First-Order Ambisonics** rendering. A dynamic sound source moves in an orbital path across the virtual dome, where `AudioSpatializerChanOp` encodes the audio into First-Order Ambisonics B-Format ($W, X, Y, Z$). Next, `AmbisonicDecodeChanOp` decodes the soundfield to a 12-channel discrete 7.1.4 Atmos ceiling dome speaker layout with distance attenuation and realistic spatial imaging.

```
┌─────────────────────┐
│  AudioFileInChanOp  │ (Carrier Audio)
└──────────┬──────────┘
           │
           ▼
┌─────────────────────┐
│  AudioEmitterComp   │ (3D Orbit Emitter)
└──────────┬──────────┘
           │
           ▼
┌─────────────────────────────┐
│   AudioSpatializerChanOp    │ (Encodes Ambisonics B-Format: W, X, Y, Z)
└──────────┬──────────────────┘
           │
           ▼
┌─────────────────────────────┐
│    AmbisonicDecodeChanOp    │ (Decodes to 7.1.4 Atmos Ceiling Speakers)
└──────────┬──────────────────┘
           │
           ▼ (12 Planar Audio Channels)
┌─────────────────────────────┐
│ Multi-Channel Audio P.A.    │
└─────────────────────────────┘
```

---

## 2. Multi-Speaker Configurations

- **Ambisonic B-Format:** 4 channels representing omnidirectional pressure ($W$) and Cartesian figure-8 velocities ($X, Y, Z$).
- **7.1.4 Dolby Atmos:** 8 surround perimeter speakers + 4 height ceiling speakers.
