# Operator Specification: AudioListenerComp

**Family:** `Comp`  
**Type Name:** `AudioListenerComp`  
**Version:** 1.0  
**Phase Ownership:** Phase 15 (Epic 15.4)  

---

## 1. Description

`AudioListenerComp` represents the virtual listener / microphone in the 3D world, providing position and look-at orientation for spatial audio rendering.

---

## 2. Pins

### Input Pins
| Pin Name | Type | Required | Description |
|---|---|---|---|
| `in_cam` | `Comp` | No | Optional CameraComp to lock listener position and orientation to active camera |

### Output Pins
| Pin Name | Type | Description |
|---|---|---|
| `output` | `Comp` | Listener component descriptor |
| `out_state` | `Chan` | Listener trajectory channel stream (`pos_x, pos_y, pos_z, yaw, pitch`) |

---

## 3. Parameters

| Parameter | Type | Default | Range | Description |
|---|---|---|---|---|
| `position` | `vec3` | `[0.0, 0.0, 0.0]` | - | Listener world position |
| `orientation` | `vec3` | `[0.0, 0.0, 0.0]` | - | Look-at orientation pitch/yaw/roll |

---

## 4. Cook Semantics

- **Time-Dependent:** No.
