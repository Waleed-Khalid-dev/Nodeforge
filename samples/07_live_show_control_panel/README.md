# Neo Realms — Master Live Show Control Surface

**Project:** `samples/07_live_show_control_panel/live_show_control_panel.nfp`  
**Target Hardware:** Touchscreen Operator Console / Stage Control Surface  
**Primary Engine Features:** `PanelComp`, `SliderPanelComp`, `ButtonPanelComp`, `DialPanelComp`, `TextEntryPanelComp`, `NoiseTexOp`, `LevelTexOp`, `ToWindowTexOp`.

---

## 1. Overview

This project provides a comprehensive in-graph **Live Show Control Surface**. It allows lighting designers and visual operators to trigger show states, modulate master brightness/dimming, adjust particle turbulence speeds via rotary knobs, and monitor visual output in real-time.

```
┌────────────────────┐   ┌────────────────────┐   ┌────────────────────┐
│  ButtonPanelComp   │   │  SliderPanelComp   │   │   DialPanelComp    │
│ (Mainstage Toggle) │   │  (Master Dimmer)   │   │(Turbulence Speed)  │
└─────────┬──────────┘   └─────────┬──────────┘   └─────────┬──────────┘
          │                        │                        │
          └────────────────────────┼────────────────────────┘
                                   │
                                   ▼
                         ┌────────────────────┐
                         │     PanelComp      │ (Root Layout Container)
                         └─────────┬──────────┘
                                   │
                                   ▼ (Real-Time Control Channels)
                         ┌────────────────────┐
                         │   Generative DAG   │ (Modulates downstream visuals)
                         └────────────────────┘
```

---

## 2. Key Controls

- **Mainstage Active:** Toggle switch latching show visual generation.
- **Master Dimmer:** Linear fader scaling global output brightness $[0.0 .. 1.0]$.
- **Turbulence Speed:** Rotary dial adjusting generative noise frequency.
- **Show Title:** Live read-only display metadata.
