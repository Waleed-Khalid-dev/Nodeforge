# Implementation Plan — Phase 15 Epic 15.3: In-Graph Interactive UI Panel Component System

**Phase:** 15 (Expansion Toward “Almost Everything”)  
**Epic:** 15.3 (In-Graph Interactive UI Panel Component System & Touch Surfaces)  
**Status:** In Planning  
**Target Milestone:** M7 (Advanced Visual FX & Dynamic Tools)  
**Company:** Neo Realms  
**Owner:** Waleed Khalid  

---

## 1. Overview & Objectives

Epic 15.3 introduces a native, modular **In-Graph UI Panel Component System** to NodeForge. Engineered to allow creative technologists and live show operators to build custom interactive touchscreen dashboards, mixing consoles, cue trigger surfaces, and on-site projector calibration panels directly inside the node graph.

Key capabilities:
- **`PanelComp`**: Root container component supporting absolute pixel positioning or auto-flow flex layouts (rows/columns/spacing), background styling, and composite texture rendering.
- **Interactive Widget Suite (`Comp` / `ChanOp` / `TexOp` hybrid)**:
  - `SliderPanelComp`: 1D horizontal/vertical continuous faders with custom min/max/step ranges and logarithmic/linear curves.
  - `ButtonPanelComp`: Momentary push, toggle latch, and multi-option radio group buttons.
  - `DialPanelComp`: Rotary 270° potentiometers and bipolar center-zero knobs.
  - `TextEntryPanelComp`: Editable text input fields and dynamic numeric value readouts.
- **Dual Graph Interoperability**:
  - Direct wire outputs (`out_chan` pin) carrying real-time values (`value, state, u, v, pressed`).
  - Python two-way parameter binding (`op('slider1')['val']`).
  - External show control feedback (`in_chan` pin) synchronizing with motorized MIDI faders and OSC network controllers.
- **Composite Texture Rendering**: Renders composite UI panels to a 2D GPU `Texture2D` (`TexOp` pin) for direct projection mapping onto physical surfaces or presentation in floating multi-touch windows.

---

## 2. Architecture & Data Structures (ADR-0016)

### 2.1 UI Panel Data Model
Every panel widget exposes normalized interaction state:

```cpp
struct PanelState {
    float value = 0.0f;          // Primary scalar value in user range [min .. max]
    float normalizedValue = 0.0f;// Normalized value [0.0 .. 1.0]
    bool pressed = false;        // Active mouse/touch press state
    bool hovered = false;        // Mouse hover state
    bool toggleState = false;    // Latch state for toggle buttons
    glm::vec2 touchUV{0.0f};     // Local normalized touch coordinate [0..1]
    std::string textValue;       // String content for text entry
};
```

### 2.2 Layout Engine
`PanelComp` executes a 2-pass layout calculation:
1. **Measurement Pass:** Computes child widget bounds based on `layout_mode` (`0: Absolute`, `1: Flex Row`, `2: Flex Column`, `3: Grid Matrix`).
2. **Hit-Testing & Dispatch Pass:** Maps window/mouse coordinates $(X, Y)$ to targeted child widgets, updating normalized interaction states and emitting `ChannelBuffer` streams.

---

## 3. Operator Suite & Specifications

### 1. `PanelComp` (Family: `Comp`)
- **Input Pins:** `in_ui` (Comp, multi-input for child widgets), `bg_tex` (Tex, optional background texture).
- **Output Pins:** `out_ui` (Comp), `out_tex` (Tex, rendered UI panel texture), `out_chan` (Chan, aggregated child values).
- **Parameters:**
  - `panel_width`: `int` width in pixels (default: `800`).
  - `panel_height`: `int` height in pixels (default: `600`).
  - `layout_mode`: `0: Absolute`, `1: Flex Row`, `2: Flex Column`, `3: Grid Matrix`.
  - `spacing`: `float` spacing between auto-layout children in pixels (default: `10.0`).
  - `padding`: `float` inner border padding in pixels (default: `12.0`).
  - `bg_color`: `vec4` panel background color (default: `[0.08, 0.09, 0.12, 0.95]`).
  - `border_color`: `vec4` border stroke color (default: `[0.2, 0.25, 0.35, 1.0]`).
  - `border_radius`: `float` corner rounding in pixels (default: `8.0`).

### 2. `SliderPanelComp` (Family: `Comp`)
- **Input Pins:** `in_chan` (Chan, optional hardware feedback input).
- **Output Pins:** `output` (Comp), `out_chan` (Chan, `val, norm_val, pressed, u`).
- **Parameters:**
  - `label`: `string` display text (default: `"Fader"`).
  - `orientation`: `0: Horizontal`, `1: Vertical`.
  - `min_value`: `float` minimum range (default: `0.0`).
  - `max_value`: `float` maximum range (default: `1.0`).
  - `value`: `float` current value (default: `0.5`).
  - `step`: `float` discrete step snapping (default: `0.0` for smooth continuous).
  - `accent_color`: `vec4` fader fill color (default: `[0.0, 0.8, 1.0, 1.0]`).

### 3. `ButtonPanelComp` (Family: `Comp`)
- **Input Pins:** `in_chan` (Chan, optional trigger input).
- **Output Pins:** `output` (Comp), `out_chan` (Chan, `state, pulse, pressed`).
- **Parameters:**
  - `label`: `string` button text (default: `"Trigger"`).
  - `button_type`: `0: Momentary`, `1: Toggle Latch`, `2: Radio Group`.
  - `state`: `bool` active toggle state (default: `false`).
  - `group_id`: `int` radio mutual exclusion group identifier (default: `0`).
  - `active_color`: `vec4` pressed/on color (default: `[0.2, 0.9, 0.4, 1.0]`).

### 4. `DialPanelComp` (Family: `Comp`)
- **Input Pins:** `in_chan` (Chan, optional feedback).
- **Output Pins:** `output` (Comp), `out_chan` (Chan, `val, angle, norm_val`).
- **Parameters:**
  - `label`: `string` dial text (default: `"Knob"`).
  - `min_value`: `float` minimum value (default: `0.0`).
  - `max_value`: `float` maximum value (default: `100.0`).
  - `value`: `float` current value (default: `50.0`).
  - `bipolar`: `bool` center-zero bipolar dial (default: `false`).
  - `sweep_angle`: `float` total rotation sweep in degrees (default: `270.0`).

### 5. `TextEntryPanelComp` (Family: `Comp`)
- **Input Pins:** `in_text` (Data, optional string source).
- **Output Pins:** `output` (Comp), `out_data` (Data, table with string value).
- **Parameters:**
  - `label`: `string` field label (default: `"Text Input"`).
  - `text`: `string` current string value (default: `""`).
  - `read_only`: `bool` display-only readout mode (default: `false`).

---

## 4. Planned File Structure

```
d:\[Project]\Touch Designer\
├── docs\
│   ├── adr\
│   │   └── ADR-0016-ui-panel-component-system.md
│   ├── operator-spec\
│   │   ├── PanelComp.md
│   │   ├── SliderPanelComp.md
│   │   ├── ButtonPanelComp.md
│   │   ├── DialPanelComp.md
│   │   └── TextEntryPanelComp.md
│   └── cheat-sheets\
│       └── 09_ui_panels_cheat_sheet.md
├── src\
│   └── operators\
│       └── comp\
│           ├── PanelComp.h/.cpp
│           ├── SliderPanelComp.h/.cpp
│           ├── ButtonPanelComp.h/.cpp
│           ├── DialPanelComp.h/.cpp
│           └── TextEntryPanelComp.h/.cpp
├── samples\
│   └── 07_live_show_control_panel\
│       ├── live_show_control_panel.nfp  # Flagship master show control surface
│       └── README.md
└── tests\
    ├── unit\
    │   └── panel_system_test.cpp        # Unit tests for hierarchy, layout, and binding
    └── benchmark\
        └── panel_benchmark.cpp          # 500-widget high-frequency update benchmark
```

---

## 5. Detailed Task Breakdown

### Task 15.3.1: Architecture Documentation & Operator Specifications
- **Agent:** `project-planner` / `architecture`
- **Priority:** P0 (Design Pre-requisite)
- **Output:**
  1. `docs/adr/ADR-0016-ui-panel-component-system.md`
  2. `docs/operator-spec/PanelComp.md`
  3. `docs/operator-spec/SliderPanelComp.md`
  4. `docs/operator-spec/ButtonPanelComp.md`
  5. `docs/operator-spec/DialPanelComp.md`
  6. `docs/operator-spec/TextEntryPanelComp.md`
  7. `docs/cheat-sheets/09_ui_panels_cheat_sheet.md`

### Task 15.3.2: UI Panel Operator Implementations
- **Agent:** `backend-specialist` / `frontend-specialist`
- **Priority:** P0 (Core Foundation)
- **Output:**
  1. `src/operators/comp/PanelComp.h/.cpp`
  2. `src/operators/comp/SliderPanelComp.h/.cpp`
  3. `src/operators/comp/ButtonPanelComp.h/.cpp`
  4. `src/operators/comp/DialPanelComp.h/.cpp`
  5. `src/operators/comp/TextEntryPanelComp.h/.cpp`
  6. Register all 5 operators in `src/graph/CoreNodes.cpp` and `CMakeLists.txt`.

### Task 15.3.3: Showcase Sample Project (`samples/07_live_show_control_panel/`)
- **Agent:** `project-planner` / `backend-specialist`
- **Priority:** P1 (Production Sample)
- **Output:**
  1. `samples/07_live_show_control_panel/live_show_control_panel.nfp`
  2. `samples/07_live_show_control_panel/README.md`

### Task 15.3.4: Automated Unit Tests & 500-Widget Benchmark
- **Agent:** `test-engineer` / `backend-specialist`
- **Priority:** P0 (CI Quality Gate)
- **Output:**
  1. `tests/unit/panel_system_test.cpp`
  2. `tests/benchmark/panel_benchmark.cpp`
  3. Update `tests/CMakeLists.txt`.

---

## 6. Phase X: Final Verification Checklist

- [ ] `docs/adr/ADR-0016-ui-panel-component-system.md` written and approved
- [ ] Operator specs for all 5 panel operators created in `docs/operator-spec/`
- [ ] `PanelComp`, `SliderPanelComp`, `ButtonPanelComp`, `DialPanelComp`, `TextEntryPanelComp` implemented
- [ ] Operators registered in `CoreNodes.cpp` and `CMakeLists.txt`
- [ ] `samples/07_live_show_control_panel/live_show_control_panel.nfp` created with documentation
- [ ] `tests/unit/panel_system_test.cpp` and `tests/benchmark/panel_benchmark.cpp` added to CMake
- [ ] 500-widget benchmark verified with sub-millisecond cook times and zero memory leaks
- [ ] All automated tests passing (100% pass rate)
- [ ] `STATUS.md` and `.agent/memory/MEMORY.md` updated
