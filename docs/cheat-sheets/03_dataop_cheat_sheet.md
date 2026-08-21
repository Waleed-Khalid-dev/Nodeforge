# DataOp Family Cheat Sheet (2D Tables, Text, JSON, Protocols & Scripting)

**Family:** `DataOp` | **Color:** Magenta (`#F50057`) | **Data Structure:** `DataTable` (2D String Grid & Structured Buffers)

---

## 1. DataOp Architecture

DataOps handle non-graphics tabular data, text strings, JSON documents, serial ports, and user-defined Python scripting hooks (`onCook` and `onPulse`).

---

## 2. Core Operator Reference (11 Operators)

| Operator | In Pins | Out Pins | Key Parameters | Cook Behavior |
|----------|---------|----------|----------------|---------------|
| **`TextDataOp`** | None | `output` (Data) | `text` (string) | Multi-line plain text or code block storage |
| **`TableDataOp`** | None | `output` (Data) | `rows` (int), `cols` (int), `file_path` (CSV/TSV) | Editable 2D table grid with CSV/TSV parser |
| **`ScriptDataOp`** | `input` (Data) | `output` (Data) | `script` (string), `run_on_cook` (bool), `pulse` (pulse) | Executes custom Python code on evaluate/pulse |
| **`JSONDataOp`** | `input` (Data) | `output` (Data) | `pointer_query` (string), `format_output` (bool) | Parses RFC 6901 JSON pointer queries |
| **`WebDataOp`** | None | `output` (Data) | `url` (string), `method` (GET/POST), `poll_interval` | Asynchronous non-blocking HTTP REST client |
| **`OSCInOp`** | None | `output` (Data) | `port` (int), `address_pattern` (string) | Logs incoming OSC packets into a live table |
| **`OSCOutOp`** | `input` (Data) | None | `ip_address` (string), `port` (int) | Sends row/column data formatted as OSC messages |
| **`SelectDataOp`** | `input` (Data) | `output` (Data) | `row_start`, `row_end`, `col_start`, `col_end` | Slices and crops tabular rows and columns |
| **`MergeDataOp`** | `input_a`, `input_b` | `output` (Data) | `direction` (AppendRows, AppendCols) | Concatenates two tables into a single table |
| **`ChanToDataOp`** | `input` (Chan) | `output` (Data) | `include_names` (bool) | Converts 1D channel buffers into a 2D data table |
| **`DataToChanOp`** | `input` (Data) | `output` (Chan) | `first_row_names` (bool) | Extracts numeric columns from tables into ChanOps |
| **`SerialDataOp`** | None | `output` (Data) | `port` (string, e.g. COM3), `baud_rate` (int) | Asynchronous Win32 overlapped Serial I/O |

---

## 3. Python ScriptDataOp Callback Standard

Inside a `ScriptDataOp`, you can define custom Python callbacks:

```python
def onCook(dat):
    """
    Called whenever input nodes change or parameters update.
    dat: the ScriptDataOp instance
    """
    dat.clear()
    dat.appendRow(["device_id", "status", "uptime"])
    dat.appendRow(["sensor_01", "ONLINE", str(me.time)])

def onPulse(dat, paramName):
    """
    Called when a pulse parameter button is clicked in the UI.
    """
    if paramName == "reset_counter":
        op('counter_state')[0, 0] = "0"
```
