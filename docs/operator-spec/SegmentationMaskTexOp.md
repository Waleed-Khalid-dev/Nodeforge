# Operator Specification: SegmentationMaskTexOp

**Family:** `TexOp`  
**Type Name:** `SegmentationMaskTexOp`  
**Version:** 1.0  
**Phase Ownership:** Phase 15 (Epic 15.9)  

---

## 1. Description

`SegmentationMaskTexOp` performs green-screen-free deep learning foreground human segmentation, outputting an alpha matte and composite RGBA subject texture.

---

## 2. Pins

### Input Pins
| Pin Name | Type | Required | Description |
|---|---|---|---|
| `input` | `Tex` | Yes | Source camera or video texture |

### Output Pins
| Pin Name | Type | Description |
|---|---|---|
| `output` | `Tex` | Foreground subject with transparent alpha |
| `out_mask` | `Tex` | Grayscale 1-channel segmentation matte |

---

## 3. Parameters

| Parameter | Type | Default | Range | Description |
|---|---|---|---|---|
| `threshold` | `float` | `0.5` | `0.0 .. 1.0` | Probability cutoff threshold |
| `feather` | `float` | `2.0` | `0.0 .. 10.0` | Edge softening blur radius |

---

## 4. Cook Semantics

- **Time-Dependent:** Yes.
