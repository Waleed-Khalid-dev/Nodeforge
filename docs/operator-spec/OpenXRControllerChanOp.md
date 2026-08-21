# Operator Specification: OpenXRControllerChanOp

**Family:** `ChanOp`  
**Type Name:** `OpenXRControllerChanOp`  
**Version:** 1.0  
**Phase Ownership:** Phase 15 (Epic 15.6)  

---

## 1. Description

`OpenXRControllerChanOp` captures 6-DOF spatial pose tracking (`tx, ty, tz, rx, ry, rz, rw`), analog triggers, thumbsticks, and digital buttons from Left and Right VR/AR motion controllers.

---

## 2. Pins

### Input Pins
None.

### Output Pins
| Pin Name | Type | Description |
|---|---|---|
| `out_left` | `Chan` | Left controller tracking & input channel stream |
| `out_right` | `Chan` | Right controller tracking & input channel stream |

---

## 3. Parameters

| Parameter | Type | Default | Range | Description |
|---|---|---|---|---|
| `hand_select` | `int` | `0` | `0 .. 2` | 0: Both Hands, 1: Left Only, 2: Right Only |
| `haptic_pulse` | `float` | `0.0` | `0.0 .. 1.0` | Controller vibration feedback amplitude |

---

## 4. Cook Semantics

- **Time-Dependent:** Yes.
