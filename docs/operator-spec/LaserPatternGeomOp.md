# Operator Specification: LaserPatternGeomOp

**Family:** `GeomOp`  
**Type Name:** `LaserPatternGeomOp`  
**Version:** 1.0  
**Phase Ownership:** Phase 15 (Epic 15.8)  

---

## 1. Description

`LaserPatternGeomOp` procedurally generates classic laser show vector patterns: Lissajous curves, spirographs, multi-beam fans, and geometric abstract tunnels.

---

## 2. Pins

### Input Pins
None.

### Output Pins
| Pin Name | Type | Description |
|---|---|---|
| `output` | `Geom` | Generated procedural laser vector path geometry |

---

## 3. Parameters

| Parameter | Type | Default | Range | Description |
|---|---|---|---|---|
| `pattern_type` | `int` | `0` | `0 .. 3` | 0: Lissajous, 1: Spirograph, 2: Beam Fan, 3: Tunnel |
| `frequency_a` | `float` | `3.0` | `0.1 .. 50.0` | X frequency ratio |
| `frequency_b` | `float` | `2.0` | `0.1 .. 50.0` | Y frequency ratio |
| `phase` | `float` | `0.0` | - | Pattern phase offset |
| `beam_count` | `int` | `8` | `2 .. 64` | Number of radial beams / fans |

---

## 4. Cook Semantics

- **Time-Dependent:** Yes.
