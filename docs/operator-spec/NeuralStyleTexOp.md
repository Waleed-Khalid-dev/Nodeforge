# Operator Specification: NeuralStyleTexOp

**Family:** `TexOp`  
**Type Name:** `NeuralStyleTexOp`  
**Version:** 1.0  
**Phase Ownership:** Phase 15 (Epic 15.9)  

---

## 1. Description

`NeuralStyleTexOp` performs real-time feedforward neural style transfer on video frames, converting input imagery into artistic aesthetics (Candy, Mosaic, Rain Princess, Udnie, Cyberpunk Neon, Ink Wash).

---

## 2. Pins

### Input Pins
| Pin Name | Type | Required | Description |
|---|---|---|---|
| `input` | `Tex` | Yes | Source camera or video texture |

### Output Pins
| Pin Name | Type | Description |
|---|---|---|
| `output` | `Tex` | Stylized output texture |

---

## 3. Parameters

| Parameter | Type | Default | Range | Description |
|---|---|---|---|---|
| `style_preset` | `int` | `4` | `0 .. 5` | 0: Candy, 1: Mosaic, 2: Rain Princess, 3: Udnie, 4: Cyberpunk Neon, 5: Ink Wash |
| `style_weight` | `float` | `1.0` | `0.0 .. 1.0` | Blend ratio between original and styled output |
| `preserve_colors` | `bool` | `false` | `true/false` | Retain original source color luminance |

---

## 4. Cook Semantics

- **Time-Dependent:** Yes.
