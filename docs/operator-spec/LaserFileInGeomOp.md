# Operator Specification: LaserFileInGeomOp

**Family:** `GeomOp`  
**Type Name:** `LaserFileInGeomOp`  
**Version:** 1.0  
**Phase Ownership:** Phase 15 (Epic 15.8)  

---

## 1. Description

`LaserFileInGeomOp` reads standard ILDA vector animation files (`.ild` format) and streams vector point frames into the NodeForge geometry pipeline.

---

## 2. Pins

### Input Pins
None.

### Output Pins
| Pin Name | Type | Description |
|---|---|---|
| `output` | `Geom` | Loaded ILDA vector frame geometry |

---

## 3. Parameters

| Parameter | Type | Default | Range | Description |
|---|---|---|---|---|
| `file_path` | `string` | `""` | - | Path to `.ild` vector file |
| `play_mode` | `int` | `0` | `0 .. 2` | 0: Loop, 1: Hold, 2: Scrub |
| `speed` | `float` | `1.0` | `-10.0 .. 10.0` | Frame playback speed multiplier |

---

## 4. Cook Semantics

- **Time-Dependent:** Yes.
