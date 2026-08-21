# UI Panel Component Subsystem Cheat Sheet

**Subsystem:** In-Graph UI Panels | **Version:** 1.0 (Phase 15 Epic 15.3) | **Family:** `Comp`

---

## 1. Widget Family Reference

| Operator | Family | In Pins | Out Pins | Key Roles |
|---|---|---|---|---|
| **`PanelComp`** | `Comp` | `in_ui` (Comp), `bg_tex` (Tex) | `out_ui` (Comp), `out_chan` (Chan), `out_tex` (Tex) | Master layout container (Absolute / Flex layout) |
| **`SliderPanelComp`** | `Comp` | `in_chan` (Chan) | `output` (Comp), `out_chan` (Chan) | Continuous / stepped 1D fader |
| **`ButtonPanelComp`** | `Comp` | `in_chan` (Chan) | `output` (Comp), `out_chan` (Chan) | Momentary, Toggle latch, or Radio group |
| **`DialPanelComp`** | `Comp` | `in_chan` (Chan) | `output` (Comp), `out_chan` (Chan) | Rotary 270° and bipolar knob |
| **`TextEntryPanelComp`**| `Comp` | `in_text` (Data) | `output` (Comp), `out_data` (Data) | Editable text box / dynamic readout |

---

## 2. Standard Network Wiring Idiom

```
# Interactive Mixing & Control Dashboard
SliderPanelComp (Master Brightness) ──┐
ButtonPanelComp (Cue Strobe Trigger) ─┼──► PanelComp (Root Dashboard) ──► ToWindowTexOp
DialPanelComp (Turbulence Speed) ─────┘         │
                                                ▼ (Channel Stream: `val, state, angle`)
                                           [Downstream Generative Graph]
```
