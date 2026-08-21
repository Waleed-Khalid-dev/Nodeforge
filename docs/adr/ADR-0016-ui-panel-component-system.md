# ADR-0016: In-Graph Interactive UI Panel Component System

**Status:** Accepted  
**Date:** 2026-08-21  
**Author:** Waleed Khalid  
**Deciders:** Core Architecture Team  
**Phase:** 15 (Expansion Toward “Almost Everything” — Epic 15.3)  

---

## 1. Context & Problem Statement

Live performance operators, projection mapping technicians, and gallery visitors need custom interactive user interfaces (sliders, buttons, rotary dials, text fields, and calibration touch panels).

Writing custom UI code outside the graph breaks the visual programming paradigm and prevents creative technologists from building self-contained, interactive control surfaces. NodeForge requires a native In-Graph UI Panel Component System (`PanelComp`) that allows users to assemble interactive control surfaces inside the node DAG, wire widget outputs directly into operators, bind them via Python expressions, and render them to 2D GPU textures for projection mapping or standalone touchscreen displays.

---

## 2. Decision & Architecture

### 2.1 Container & Widget Hierarchy (`PanelComp`)
`PanelComp` inherits from `Node` (Family: `Comp`) and acts as a layout container:

- **Layout Modes:**
  1. `0: Absolute` — Explicit $(X, Y, W, H)$ pixel positioning.
  2. `1: Flex Row` — Horizontal auto-flow with configurable spacing.
  3. `2: Flex Column` — Vertical auto-flow with configurable spacing.
  4. `3: Grid Matrix` — Multi-column uniform grid layout.

### 2.2 Normalized Interaction State Model
Every child widget maintains an internal `PanelState`:

```cpp
struct PanelState {
    float value = 0.0f;           // Current user-range value
    float normalizedValue = 0.0f; // Normalized [0.0 .. 1.0]
    bool pressed = false;         // Active touch/mouse press
    bool hovered = false;         // Mouse hover state
    bool toggleState = false;     // Latch state for toggle buttons
    glm::vec2 touchUV{0.0f};      // Normalized touch coordinate
    std::string textValue;        // Text entry string
};
```

### 2.3 Dual Output Pipeline & Hardware Interop
1. **Direct Channels (`out_chan`):** Widgets emit a `ChannelBuffer` with channels (`value`, `norm_val`, `state`, `pressed`).
2. **Two-Way Python Binding:** Expression `op('slider1')['val']` accesses current state with zero cook overhead.
3. **Hardware Controller Sync (`in_chan`):** Incoming MIDI CC or OSC streams update widget values bidirectionally.
4. **GPU Composite Texture (`out_tex`):** Renders the composite interactive UI into a 2D GPU `Texture2D` for projection mapping or window display.

---

## 3. Consequences

### Positive
- **Fully In-Graph UI Creation:** No web browser or external code required.
- **Microsecond Cook Times:** Channel updates take < 0.01 ms per widget.
- **Projection Mapping UI:** Panels can be warped and projection-mapped onto physical objects via `WarpBlendTexOp`.

### Negative / Mitigations
- **Rendering Overhead:** Rasterizing UI panels to textures on CPU/GPU is executed on-demand only when `out_tex` is wired.
