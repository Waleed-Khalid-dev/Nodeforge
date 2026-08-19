# TableDataOp

## Overview
`TableDataOp` manages 2D tabular data grids with support for rows, columns, headers, and CSV/TSV import and export.

## Category
- **Family:** `DataOp` (DAT)
- **C++ Class:** `nf::TableDataOp`
- **Output Type:** `PinType::Data` (`DataTable`)

## Parameters

| Parameter | Type | Default | Min | Max | Description |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `file_path` | `String` | `""` | - | - | Path to external `.csv` or `.tsv` file. |
| `delimiter` | `String` | `","` | - | - | Cell delimiter character (comma or tab). |
| `has_headers` | `Bool` | `true` | - | - | Treat first row as column header names. |
| `raw_csv` | `String` | `""` | - | - | Embedded CSV text content. |

## Inputs / Outputs
- **Inputs:** None (Generator).
- **Outputs:**
  - `output` (`PinType::Data`): Output `DataTable`.
