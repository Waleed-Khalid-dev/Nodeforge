# Spatial Audio & Ambisonics Subsystem Cheat Sheet

**Subsystem:** 3D Spatial Audio & Multi-Speaker Ambisonics | **Version:** 1.0 (Phase 15 Epic 15.4)

---

## 1. Spatial Audio Operator Reference

| Operator | Family | In Pins | Out Pins | Key Roles |
|---|---|---|---|---|
| **`AudioSpatializerChanOp`** | `ChanOp` | `in_audio` (Chan), `in_pos` (Chan) | `out_audio` (Chan) | 3D sound panner to 5.1/7.1.4 Atmos or B-Format ($W,X,Y,Z$) |
| **`AmbisonicDecodeChanOp`** | `ChanOp` | `in_bformat` (Chan), `speaker_layout_data` (Data) | `out_speakers` (Chan) | Decodes B-Format soundfields to physical speakers |
| **`AudioEmitterComp`** | `Comp` | `in_audio` (Chan), `in_geom` (Geom) | `output` (Comp), `out_audio` (Chan) | 3D scene spatial sound emitter |
| **`AudioListenerComp`** | `Comp` | `in_cam` (Comp) | `output` (Comp), `out_state` (Chan) | 3D virtual listener / microphone |

---

## 2. Standard Network Wiring Idiom

```
# 3D Planetarium Dome Spatial Audio Installation
AudioFileInChanOp (Synth Loop) ────► AudioEmitterComp (Orbiting Sphere)
                                             │
                                             ▼ (Audio + Position Channels)
AudioListenerComp (Center Dome) ───► AudioSpatializerChanOp (7.1.4 Atmos Dome)
                                             │
                                             ▼ (12 Discrete Speaker Channels)
                                      [Audio Output Device / Stage P.A.]
```
