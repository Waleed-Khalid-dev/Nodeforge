# TextDataOp

## Overview
`TextDataOp` stores, loads, and edits raw multiline UTF-8 text strings (e.g. GLSL shaders, documentation, configuration files, script templates).

## Category
- **Family:** `DataOp` (DAT)
- **C++ Class:** `nf::TextDataOp`
- **Output Type:** `PinType::Data` (`DataTable` single-column multiline representation)

## Parameters

| Parameter | Type | Default | Min | Max | Description |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `text` | `String` | `""` | - | - | Raw multiline text content. |
| `file_path` | `String` | `""` | - | - | External file path to load on cook/reload. |
| `auto_reload` | `Bool` | `false` | - | - | Watch external file and reload on change. |

## Inputs / Outputs
- **Inputs:** None (Generator).
- **Outputs:**
  - `output` (`PinType::Data`): Resulting text wrapped in `DataTable`.
