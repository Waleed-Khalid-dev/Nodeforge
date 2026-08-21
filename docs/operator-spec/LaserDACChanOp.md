# Operator Specification: LaserDACChanOp

**Family:** `ChanOp`  
**Type Name:** `LaserDACChanOp`  
**Version:** 1.0  
**Phase Ownership:** Phase 15 (Epic 15.8)  

---

## 1. Description

`LaserDACChanOp` streams ILDA vector point channel data to Ether Dream (Ethernet) or Helios (USB) hardware DACs with real-time buffer management and safety shutter control.

---

## 2. Pins

### Input Pins
| Pin Name | Type | Required | Description |
|---|---|---|---|
| `in_points` | `Chan` | Yes | Laser points channel stream (`x, y, r, g, b, blank`) |

### Output Pins
| Pin Name | Type | Description |
|---|---|---|
| `out_telemetry` | `Chan` | Buffer health telemetry (`buffer_fill, pps, dropped`) |

---

## 3. Parameters

| Parameter | Type | Default | Range | Description |
|---|---|---|---|---|
| `dac_type` | `int` | `2` | `0 .. 2` | 0: Ether Dream, 1: Helios USB, 2: Emulated Headless |
| `ip_address` | `string` | `"192.168.1.100"` | - | Ether Dream DAC IP address |
| `safety_shutter` | `bool` | `true` | `true/false` | Hardware laser safety shutter enable |
| `master_brightness` | `float` | `1.0` | `0.0 .. 1.0` | Master laser power scaler |

---

## 4. Cook Semantics

- **Time-Dependent:** Yes.
