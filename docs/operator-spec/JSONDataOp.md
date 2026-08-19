# JSONDataOp

## Overview
`JSONDataOp` parses JSON strings or files, performs JSON Pointer queries (`/root/array/0/key`), and converts between hierarchical JSON structures and tabular key-value rows.

## Category
- **Family:** `DataOp` (DAT)
- **C++ Class:** `nf::JSONDataOp`
- **Output Type:** `PinType::Data` (`DataTable`)

## Parameters

| Parameter | Type | Default | Min | Max | Description |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `json_text` | `String` | `"{}"` | - | - | Raw JSON string. |
| `file_path` | `String` | `""` | - | - | External `.json` file to load. |
| `json_pointer` | `String` | `""` | - | - | JSON Pointer query path (e.g. `/devices/0/ip`). |
| `mode` | `Int` | `0` | 0 | 2 | 0: Raw Text, 1: Key-Value Table, 2: Array Table. |

## Inputs / Outputs
- **Inputs:**
  - `input` (`PinType::Data`, optional): Input JSON text data.
- **Outputs:**
  - `output` (`PinType::Data`): Parsed JSON representation in `DataTable`.
