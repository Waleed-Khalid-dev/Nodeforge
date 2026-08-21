# Operator Specification: AudioSpatializerChanOp

**Family:** `ChanOp`  
**Type Name:** `AudioSpatializerChanOp`  
**Version:** 1.0  
**Phase Ownership:** Phase 15 (Epic 15.4)  

---

## 1. Description

`AudioSpatializerChanOp` takes input audio channels and spatial coordinates, performing real-time 3D panning across discrete multi-speaker layouts (Stereo, Quad 4.0, 5.1 Surround, 7.1.4 Atmos, 16-ch array) or encoding into First-Order Ambisonics ($W, X, Y, Z$) B-Format.

---

## 2. Pins

### Input Pins
| Pin Name | Type | Required | Description |
|---|---|---|---|
| `in_audio` | `Chan` | Yes | Monophonic or stereo source audio stream |
| `in_pos` | `Chan` | No | Optional dynamic 3D emitter trajectory channels (`tx, ty, tz`) |

### Output Pins
| Pin Name | Type | Description |
|---|---|---|
| `out_audio` | `Chan` | Multi-channel spatialized output stream (discrete speakers or Ambisonic $W, X, Y, Z$) |

---

## 3. Parameters

| Parameter | Type | Default | Range | Description |
|---|---|---|---|---|
| `spatial_mode` | `int` | `1` | `0 .. 1` | 0: Ambisonics B-Format ($W,X,Y,Z$), 1: VBAP Multi-Speaker Layout |
| `speaker_layout` | `int` | `2` | `0 .. 5` | 0: Stereo, 1: Quad 4.0, 2: 5.1 Surround, 3: 7.1.4 Atmos, 4: Octaphonic 8.0, 5: 16-Ch |
| `emitter_pos` | `vec3` | `[0.0, 0.0, 0.0]` | - | Emitter 3D world coordinates |
| `listener_pos` | `vec3` | `[0.0, 0.0, 0.0]` | - | Listener 3D world coordinates |
| `listener_rot` | `vec3` | `[0.0, 0.0, 0.0]` | - | Listener orientation pitch/yaw/roll in degrees |
| `attenuation_rolloff` | `float` | `1.0` | `0.0 .. 10.0` | Distance attenuation rate |
| `max_distance` | `float` | `100.0` | `1.0 .. 10000.0` | Maximum audible radius |
| `doppler_factor` | `float` | `1.0` | `0.0 .. 5.0` | Doppler pitch shift factor |

---

## 4. Cook Semantics

- **Time-Dependent:** Yes (updates continuously as emitter/listener positions move).
