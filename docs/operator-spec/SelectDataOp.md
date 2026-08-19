# SelectDataOp

## Overview
`SelectDataOp` filters and extracts specific rows, columns, or cells from an input `DataTable` using integer ranges, header names, wildcard patterns, or regular expressions.

## Category
- **Family:** `DataOp` (DAT)
- **C++ Class:** `nf::SelectDataOp`
- **Output Type:** `PinType::Data` (`DataTable`)

## Parameters

| Parameter | Type | Default | Min | Max | Description |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `row_pattern` | `String` | `"*"` | - | - | Row filter pattern or range (`0-10`, `name*`, `*`). |
| `col_pattern` | `String` | `"*"` | - | - | Column filter pattern or names (`val1 val2`, `chan*`). |
| `include_headers` | `Bool` | `true` | - | - | Preserve column headers in selected output. |
| `start_row` | `Int` | `0` | 0 | 100000 | Range start row index. |
| `num_rows` | `Int` | `-1` | -1 | 100000 | Number of rows to select (-1 = all). |

## Inputs / Outputs
- **Inputs:**
  - `input` (`PinType::Data`): Upstream source table.
- **Outputs:**
  - `output` (`PinType::Data`): Filtered subset `DataTable`.
