# ScriptDataOp

## Overview
`ScriptDataOp` executes embedded Python scripts on cook or on pulse event triggers, allowing programmatic table manipulation and DAG automation.

## Category
- **Family:** `DataOp` (DAT)
- **C++ Class:** `nf::ScriptDataOp`
- **Output Type:** `PinType::Data` (`DataTable`)

## Parameters

| Parameter | Type | Default | Min | Max | Description |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `script` | `String` | `def onCook(dat):\n    pass\n` | - | - | Embedded Python script code. |
| `file_path` | `String` | `""` | - | - | External `.py` script path. |
| `cook_on_start` | `Bool` | `true` | - | - | Execute on graph initialization. |

## Python Callback API
```python
def onCook(dat):
    # dat is the output DataTable
    dat.clear()
    dat.append_row(["name", "value"])
    dat.append_row(["freq", str(440.0)])

def onPulse(dat, param_name):
    # called when a pulse parameter is triggered
    pass
```

## Inputs / Outputs
- **Inputs:**
  - `input` (`PinType::Data`, optional): Input table to read.
- **Outputs:**
  - `output` (`PinType::Data`): Resulting mutated `DataTable`.
