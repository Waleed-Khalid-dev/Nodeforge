# MergeDataOp

## Overview
`MergeDataOp` concatenates multiple input `DataTable` streams by rows (vertical append) or columns (horizontal join) with automatic alignment and collision handling.

## Category
- **Family:** `DataOp` (DAT)
- **C++ Class:** `nf::MergeDataOp`
- **Output Type:** `PinType::Data` (`DataTable`)

## Parameters

| Parameter | Type | Default | Min | Max | Description |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `mode` | `Int` | `0` | 0 | 1 | 0: Append Rows (Vertical), 1: Append Columns (Horizontal). |
| `align_headers` | `Bool` | `true` | - | - | Match columns by header names during row append. |

## Inputs / Outputs
- **Inputs:**
  - `input1` (`PinType::Data`): First input table.
  - `input2` (`PinType::Data`): Second input table.
- **Outputs:**
  - `output` (`PinType::Data`): Merged composite `DataTable`.
