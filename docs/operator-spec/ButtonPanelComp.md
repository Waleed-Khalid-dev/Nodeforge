# Operator Specification: ButtonPanelComp

**Family:** `Comp`  
**Type Name:** `ButtonPanelComp`  
**Version:** 1.0  
**Phase Ownership:** Phase 15 (Epic 15.3)  

---

## 1. Description

`ButtonPanelComp` provides a multi-mode push button widget supporting Momentary action, Toggle Latch switches, and Radio Group selectors.

---

## 2. Pins

### Input Pins
| Pin Name | Type | Required | Description |
|---|---|---|---|
| `in_chan` | `Chan` | No | Optional trigger channel setting button state |

### Output Pins
| Pin Name | Type | Description |
|---|---|---|
| `output` | `Comp` | UI widget descriptor connected into parent `PanelComp` |
| `out_chan` | `Chan` | Real-time channel output (`state`, `pulse`, `pressed`) |

---

## 3. Parameters

| Parameter | Type | Default | Range | Description |
|---|---|---|---|---|
| `label` | `string` | `"Trigger"` | - | Button display label |
| `button_type` | `int` | `0` | `0 .. 2` | 0: Momentary, 1: Toggle Latch, 2: Radio Group |
| `state` | `bool` | `false` | `true/false` | Current on/off state |
| `group_id` | `int` | `0` | `0 .. 100` | Radio group mutual exclusion ID |
| `active_color`| `vec4` | `[0.2, 0.9, 0.4, 1.0]` | `[0.0 .. 1.0]` | Active/pressed highlight color |

---

## 4. Cook Semantics

- **Time-Dependent:** No.
