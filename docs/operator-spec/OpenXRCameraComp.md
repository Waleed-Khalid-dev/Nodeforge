# Operator Specification: OpenXRCameraComp

**Family:** `Comp`  
**Type Name:** `OpenXRCameraComp`  
**Version:** 1.0  
**Phase Ownership:** Phase 15 (Epic 15.6)  

---

## 1. Description

`OpenXRCameraComp` represents the stereoscopic virtual camera rig tracked to the OpenXR HMD head pose in 3D world space.

---

## 2. Pins

### Input Pins
None.

### Output Pins
| Pin Name | Type | Description |
|---|---|---|
| `output` | `Comp` | Stereoscopic camera rig descriptor for `Scene3DPass` |
| `out_head_pose` | `Chan` | Head 6-DOF tracking stream (`tx, ty, tz, rx, ry, rz, rw`) |

---

## 3. Parameters

| Parameter | Type | Default | Range | Description |
|---|---|---|---|---|
| `tracking_space` | `int` | `1` | `0 .. 1` | 0: Local Seated, 1: Stage Room-Scale |
| `camera_offset` | `vec3` | `[0.0, 0.0, 0.0]` | - | World-space origin offset |

---

## 4. Cook Semantics

- **Time-Dependent:** Yes.
