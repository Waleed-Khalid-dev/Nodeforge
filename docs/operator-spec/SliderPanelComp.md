# Operator Specification: SliderPanelComp

**Family:** `Comp`  
**Type Name:** `SliderPanelComp`  
**Version:** 1.0  
**Phase Ownership:** Phase 15 (Epic 15.3)  

---

## 1. Description

`SliderPanelComp` provides a continuous or stepped 1D fader widget for user interfaces. It supports horizontal and vertical orientations, custom value ranges, snap steps, and hardware feedback synchronization.

---

## 2. Pins

### Input Pins
| Pin Name | Type | Required | Description |
|---|---|---|---|
| `in_chan` | `Chan` | No | Optional hardware controller channel (e.g. MIDI CC) driving slider value |

### Output Pins
| Pin Name | Type | Description |
|---|---|---|
| `output` | `Comp` | UI widget descriptor connected into parent `PanelComp` |
| `out_chan` | `Chan` | Real-time channel output (`value`, `norm_val`, `pressed`, `u`) |

---

## 3. Parameters

| Parameter | Type | Default | Range | Description |
|---|---|---|---|---|
| `label` | `string` | `"Fader"` | - | On-screen label text |
| `orientation` | `int` | `0` | `0 .. 1` | 0: Horizontal, 1: Vertical |
| `min_value` | `float` | `0.0` | `[-10000.0 .. 10000.0]` | Minimum range value |
| `max_value` | `float` | `1.0` | `[-10000.0 .. 10000.0]` | Maximum range value |
| `value` | `float` | `0.5` | `[-10000.0 .. 10000.0]` | Current slider value |
| `step` | `float` | `0.0` | `[0.0 .. 100.0]` | Step increment (0.0 for continuous smooth) |
| `accent_color`| `vec4` | `[0.0, 0.8, 1.0, 1.0]` | `[0.0 .. 1.0]` | Fader fill accent color |

---

## 4. Cook Semantics

- **Time-Dependent:** No (updates when interacted with or driven by input channels).
