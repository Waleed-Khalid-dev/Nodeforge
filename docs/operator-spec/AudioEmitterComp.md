# Operator Specification: AudioEmitterComp

**Family:** `Comp`  
**Type Name:** `AudioEmitterComp`  
**Version:** 1.0  
**Phase Ownership:** Phase 15 (Epic 15.4)  

---

## 1. Description

`AudioEmitterComp` binds an audio channel source to a 3D geometry object or world position in the 3D scene graph, setting outer attenuation radii and directional emission cones.

---

## 2. Pins

### Input Pins
| Pin Name | Type | Required | Description |
|---|---|---|---|
| `in_audio` | `Chan` | No | Audio channel to emit |
| `in_geom` | `Geom` | No | 3D geometry providing world transform |

### Output Pins
| Pin Name | Type | Description |
|---|---|---|
| `output` | `Comp` | Emitter component descriptor in the scene graph |
| `out_audio` | `Chan` | Passed through audio stream with positional channel metadata |

---

## 3. Parameters

| Parameter | Type | Default | Range | Description |
|---|---|---|---|---|
| `translate` | `vec3` | `[0.0, 0.0, 0.0]` | - | World 3D position |
| `radius` | `float` | `1.0` | `0.1 .. 1000.0` | Inner full-volume radius |
| `cone_inner_angle` | `float` | `360.0` | `0.0 .. 360.0` | Directional beam inner angle |
| `cone_outer_gain` | `float` | `0.0` | `0.0 .. 1.0` | Sound level outside emission cone |

---

## 4. Cook Semantics

- **Time-Dependent:** No.
