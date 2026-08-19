# ChanToDataOp

## Overview
`ChanToDataOp` (ChopToDat) converts an incoming `ChannelBuffer` (ChanOp stream) into a structured `DataTable` where each channel becomes a named column and each sample is a row.

## Category
- **Family:** `DataOp` (DAT)
- **C++ Class:** `nf::ChanToDataOp`
- **Output Type:** `PinType::Data` (`DataTable`)

## Parameters

| Parameter | Type | Default | Min | Max | Description |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `include_index` | `Bool` | `true` | - | - | Include sample index column (`sample`). |
| `format` | `String` | `"%.4f"` | - | - | Floating point number formatting. |
| `max_rows` | `Int` | `1000` | 1 | 100000 | Maximum rows to generate. |

## Inputs / Outputs
- **Inputs:**
  - `input` (`PinType::Chan`): Input `ChannelBuffer`.
- **Outputs:**
  - `output` (`PinType::Data`): Resulting sample grid `DataTable`.
