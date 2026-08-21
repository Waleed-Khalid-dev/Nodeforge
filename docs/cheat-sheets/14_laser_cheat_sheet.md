# Laser DAC & ILDA Laser Projector Subsystem Cheat Sheet

**Subsystem:** Laser DAC & ILDA Laser Projector Control | **Version:** 1.0 (Phase 15 Epic 15.8)

---

## 1. Laser Operator Reference

| Operator | Family | In Pins | Out Pins | Key Roles |
|---|---|---|---|---|
| **`LaserGeomOp`** | `GeomOp` | `geometry` (Geom) | `output` (Geom), `out_points` (Chan) | Converts 3D/2D vector meshes to ILDA paths with blanking/dwells |
| **`LaserDACChanOp`** | `ChanOp` | `in_points` (Chan) | `out_telemetry` (Chan) | Streams points to Ether Dream / Helios DACs with safety shutter |
| **`LaserPatternGeomOp`** | `GeomOp` | None | `output` (Geom) | Procedural laser lissajous, spirographs, and beam fans |
| **`LaserFileInGeomOp`** | `GeomOp` | None | `output` (Geom) | Loads standard ILDA `.ild` vector animation files |

---

## 2. Standard Network Wiring Idiom

```
# Live Laser Show with Audio Reactivity, Volumetric Smoke & Hardware DAC
LaserPatternGeomOp (Lissajous) ──► LaserGeomOp (Galvo Optimizer) ──┬─► LaserDACChanOp (Ether Dream DAC)
         ▲                                                         │
AudioInChanOp (Bass Deform)                                        └─► Scene3DPass (3D Volumetric Smoke)
```
