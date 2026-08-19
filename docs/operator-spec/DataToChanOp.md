# DataToChanOp

## Overview
`DataToChanOp` (DatToChop) parses numeric data from rows or columns in a `DataTable` and constructs a multi-channel `ChannelBuffer` for modulation and synthesis.

## Category
- **Family:** `DataOp` (DAT)
- **C++ Class:** `nf::DataToChanOp`
- **Output Type:** `PinType::Chan` (`ChannelBuffer`)

## Parameters

| Parameter | Type | Default | Min | Max | Description |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `layout` | `Int` | `0` | 0 | 1 | 0: Channels in Columns (Rows=Samples), 1: Channels in Rows (Cols=Samples). |
| `sample_rate` | `Float` | `60.0` | 0.1 | 192000.0 | Sample rate in Hz for output buffer. |
| `has_headers` | `Bool` | `true` | - | - | Treat first row/col as channel names. |

## Inputs / Outputs
- **Inputs:**
  - `input` (`PinType::Data`): Input `DataTable`.
- **Outputs:**
  - `output` (`PinType::Chan`): Extracted `ChannelBuffer`.
