# Operator Specification: AmbisonicDecodeChanOp

**Family:** `ChanOp`  
**Type Name:** `AmbisonicDecodeChanOp`  
**Version:** 1.0  
**Phase Ownership:** Phase 15 (Epic 15.4)  

---

## 1. Description

`AmbisonicDecodeChanOp` decodes a 4-channel First-Order Ambisonics ($W, X, Y, Z$) B-format stream into target physical loudspeaker feeds (Stereo, Quad, 5.1, 7.1.4 Atmos dome, or custom coordinate tables).

---

## 2. Pins

### Input Pins
| Pin Name | Type | Required | Description |
|---|---|---|---|
| `in_bformat` | `Chan` | Yes | 4-channel Ambisonics stream ($W, X, Y, Z$) |
| `speaker_layout_data` | `Data` | No | Optional custom speaker XYZ coordinate table |

### Output Pins
| Pin Name | Type | Description |
|---|---|---|
| `out_speakers` | `Chan` | Discrete multi-channel loudspeaker audio stream |

---

## 3. Parameters

| Parameter | Type | Default | Range | Description |
|---|---|---|---|---|
| `target_layout` | `int` | `2` | `0 .. 4` | 0: Stereo, 1: Quad 4.0, 2: 5.1 Surround, 3: 7.1.4 Atmos Dome, 4: Custom Table |
| `shelf_filter_hf` | `bool` | `true` | `true/false` | High-frequency shelf optimization |

---

## 4. Cook Semantics

- **Time-Dependent:** Yes.
