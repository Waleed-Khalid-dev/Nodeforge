# Operator Specification: ParticleAttractorGeomOp

**Family:** `GeomOp`  
**Type Name:** `ParticleAttractorGeomOp`  
**Version:** 1.0  
**Phase Ownership:** Phase 15 (Epic 15.1)  

---

## 1. Description

`ParticleAttractorGeomOp` introduces dynamic 3D spherical point attractors and repulsors to a particle stream. It is designed to bind directly to real-time interactive tracking coordinates (e.g. OSC hand gestures, mouse positions, or audio centroids).

---

## 2. Pins

### Input Pins
| Pin Name | Type | Required | Description |
|---|---|---|---|
| `input` | `Geom` | Yes | Incoming particle stream |
| `target_pos` | `Chan` | No | Optional 3-channel input (`x, y, z` or `chan1, chan2, chan3`) driving attractor position |

### Output Pins
| Pin Name | Type | Description |
|---|---|---|
| `output` | `Geom` | Updated particle stream influenced by attractor/repulsor forces |

---

## 3. Parameters

| Parameter | Type | Default | Range | Description |
|---|---|---|---|---|
| `attractor_type` | `int` | `0` | `0 .. 1` | 0: Attract (pull toward), 1: Repel (push away) |
| `position` | `vec3` | `[0.0, 0.0, 0.0]` | `[-1000.0 .. 1000.0]` | World 3D center of attraction |
| `strength` | `float` | `5.0` | `-100.0 .. 100.0` | Force acceleration magnitude |
| `radius` | `float` | `4.0` | `0.01 .. 100.0` | Outer sphere boundary of influence |
| `falloff` | `int` | `1` | `0 .. 2` | 0: Linear, 1: Inverse Square, 2: Smoothstep |
| `max_velocity` | `float` | `20.0` | `0.1 .. 200.0` | Velocity clamp preventing particle escape velocity |

---

## 4. Cook Semantics

- **Time-Dependent:** Yes. Pulls target position from channel bindings or expressions every frame.
