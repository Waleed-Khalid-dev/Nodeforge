# Operator Specification: DialPanelComp

**Family:** `Comp`  
**Type Name:** `DialPanelComp`  
**Version:** 1.0  
**Phase Ownership:** Phase 15 (Epic 15.3)  

---

## 1. Description

`DialPanelComp` provides a rotary potentiometer / knob widget with configurable angular sweep (default: 270°), optional center-zero bipolar operation, and numeric readouts.

---

## 2. Pins

### Input Pins
| Pin Name | Type | Required | Description |
|---|---|---|---|
| `in_chan` | `Chan` | No | Optional feedback channel driving knob angle |

### Output Pins
| Pin Name | Type | Description |
|---|---|---|
| `output` | `Comp` | UI widget descriptor connected into parent `PanelComp` |
| `out_chan` | `Chan` | Real-time channel output (`value`, `angle`, `norm_val`) |

---

## 3. Parameters

| Parameter | Type | Default | Range | Description |
|---|---|---|---|---|
| `label` | `string` | `"Knob"` | - | On-screen label text |
| `min_value` | `float` | `0.0` | `[-10000.0 .. 10000.0]` | Minimum range value |
| `max_value` | `float` | `100.0` | `[-10000.0 .. 10000.0]` | Maximum range value |
| `value` | `float` | `50.0` | `[-10000.0 .. 10000.0]` | Current knob value |
| `bipolar` | `bool` | `false` | `true/false` | Centers zero at top (bipolar mode) |
| `sweep_angle`| `float` | `270.0` | `30.0 .. 360.0` | Total rotation sweep in degrees |

---

## 4. Cook Semantics

- **Time-Dependent:** No.
