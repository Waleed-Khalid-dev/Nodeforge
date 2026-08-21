# Operator Specification: OpenXRHandTrackingChanOp

**Family:** `ChanOp`  
**Type Name:** `OpenXRHandTrackingChanOp`  
**Version:** 1.0  
**Phase Ownership:** Phase 15 (Epic 15.6)  

---

## 1. Description

`OpenXRHandTrackingChanOp` streams 26 articulated skeletal hand joints per hand and gesture metrics (pinch strength, grab strength, palm direction).

---

## 2. Pins

### Input Pins
None.

### Output Pins
| Pin Name | Type | Description |
|---|---|---|
| `out_skeleton` | `Chan` | 26-joint skeletal transforms per hand |
| `out_gestures` | `Chan` | Hand gesture metrics (`pinch, grab, palm_normal_xyz`) |

---

## 3. Parameters

| Parameter | Type | Default | Range | Description |
|---|---|---|---|---|
| `hand_select` | `int` | `0` | `0 .. 2` | 0: Both Hands, 1: Left Only, 2: Right Only |
| `confidence_threshold` | `float` | `0.5` | `0.0 .. 1.0` | Minimum optical tracking confidence |

---

## 4. Cook Semantics

- **Time-Dependent:** Yes.
