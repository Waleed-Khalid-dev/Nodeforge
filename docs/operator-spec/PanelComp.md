# Operator Specification: PanelComp

**Family:** `Comp`  
**Type Name:** `PanelComp`  
**Version:** 1.0  
**Phase Ownership:** Phase 15 (Epic 15.3)  

---

## 1. Description

`PanelComp` is the master container component for building in-graph user interfaces. It arranges child panel widgets (sliders, buttons, dials, text inputs) using Absolute or Flex layout modes and generates composite channel streams and optional GPU UI textures.

---

## 2. Pins

### Input Pins
| Pin Name | Type | Required | Description |
|---|---|---|---|
| `in_ui` | `Comp` | No | Child panel widgets connected into the layout container |
| `bg_tex` | `Tex` | No | Optional background texture image or generative video |

### Output Pins
| Pin Name | Type | Description |
|---|---|---|
| `out_ui` | `Comp` | Hierarchical composite UI descriptor |
| `out_chan` | `Chan` | Aggregated channel stream containing values of all child widgets |
| `out_tex` | `Tex` | Rendered 2D GPU texture of the composite control panel |

---

## 3. Parameters

| Parameter | Type | Default | Range | Description |
|---|---|---|---|---|
| `panel_width` | `int` | `800` | `100 .. 4096` | Panel surface width in pixels |
| `panel_height` | `int` | `600` | `100 .. 4096` | Panel surface height in pixels |
| `layout_mode` | `int` | `1` | `0 .. 3` | 0: Absolute, 1: Flex Row, 2: Flex Column, 3: Grid Matrix |
| `spacing` | `float` | `10.0` | `0.0 .. 100.0` | Spacing between child widgets in pixels |
| `padding` | `float` | `12.0` | `0.0 .. 100.0` | Inner border padding in pixels |
| `bg_color` | `vec4` | `[0.08, 0.09, 0.12, 0.95]` | `[0.0 .. 1.0]` | Panel background color |
| `border_color` | `vec4` | `[0.2, 0.25, 0.35, 1.0]` | `[0.0 .. 1.0]` | Border stroke color |
| `border_radius`| `float` | `8.0` | `0.0 .. 50.0` | Corner rounding radius |

---

## 4. Cook Semantics

- **Time-Dependent:** Only if child widgets or background textures are time-dependent.
- **Resource Management:** Updates cached `ChannelBuffer` representing all child values.
