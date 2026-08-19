# ADR-0008: DataOp (DAT) Pipeline, Tabular Data Model, and Script Execution Architecture

- **Status:** Accepted
- **Date:** 2026-08-19
- **Author:** Waleed Khalid / Antigravity AI
- **Context:** NodeForge Phase 8 (DataOps + Script Nodes)

---

## 1. Context and Problem Statement
NodeForge requires a first-class tabular and textual data processing family (**DataOp** / DAT). In real-time creative computing and projection mapping installations, data operators fulfill critical responsibilities:
1. **Scripting & Automation**: Executing Python code on cook, on pulse, or on external events to manipulate node parameters, spawn subnetworks, and orchestrate animations.
2. **Structured Tabular Data**: Storing, filtering, and joining 2D tables (e.g. DMX patch lists, projection surface coordinate tables, CSV configuration files, OSC message logs).
3. **Structured Interchange Formats**: Parsing and querying JSON documents with JSON Pointer notation (`/surfaces/0/points`).
4. **Network Protocols**: Asynchronous HTTP REST communication (`WebDataOp`) and low-latency UDP Open Sound Control (`OSCInOp`, `OSCOutOp`) for live sensor and mobile controller input.
5. **Operator Interoperability**: Converting time-series `ChannelBuffer` waveforms into tables and extracting table columns back into numeric control channels.

---

## 2. Architectural Decisions

### 2.1 Unified `DataTable` In-Memory Model
Instead of maintaining fragmented text and table classes, NodeForge introduces a single high-performance `nf::DataTable` class:
- **Grid Layout**: 2D vector matrix `std::vector<std::vector<std::string>>` maintaining UTF-8 string cells.
- **Header Labeling**: Optional column headers (`m_colHeaders`) and row headers (`m_rowHeaders`) with fast string-to-index lookup maps.
- **Raw Text Integration**: Single-column or multiline raw text files are represented seamlessly as line-by-line single-column tables or accessed via `GetAsText()` / `SetFromText()`.
- **Delimited I/O**: RFC 4180-compliant CSV and TSV tokenizer and serializer supporting quote escaping (`"hello, world"`).
- **Fast Numeric Accessors**: `GetCellFloat(r, c)`, `GetCellInt(r, c)` with fallback defaults.

### 2.2 Python Script Execution Model (`ScriptDataOp`)
- **Dual Triggers**:
  - `onCook(dat)`: Invoked during DAG topological evaluation when inputs or parameters are dirty.
  - `onPulse(dat, paramName)`: Invoked when pulse buttons or UI actions trigger the script.
- **Exception Boundaries**: Wrapped in `pybind11::error_already_set` exception catch blocks with `spdlog::error()` diagnostics to prevent host application termination.
- **Table Mutation API**: Direct Python binding methods (`dat.set_cell(r, c, val)`, `dat.append_row(...)`, `dat.clear()`, `dat.copy(inDat)`).

### 2.3 Network Architecture & Thread-Safe Ingest
- **OSC UDP Reception (`OSCInOp`)**:
  - Dedicated background worker thread listening on a non-blocking UDP socket.
  - Parsed OSC packets (`/address val1 val2 ...`) pushed into a thread-safe lock-free ring buffer.
  - Drained during the main thread frame start into a structured timestamped table (`timestamp`, `address`, `arg0`, `arg1`...).
- **OSC UDP Transmission (`OSCOutOp`)**:
  - Direct transmission of binary OSC packets over UDP socket on cook or trigger.
- **Asynchronous HTTP Client (`WebDataOp`)**:
  - Offloaded to `std::jthread` worker thread using native non-blocking HTTP requests.
  - Response status, headers, and body text/JSON are populated into the output table upon request completion without stalling frame rendering.

### 2.4 Operator Interoperability Bridges
- **`ChanToDataOp` (ChopToDat)**: Converts planar `ChannelBuffer` data into a table where each channel is a column and each sample is a row.
- **`DataToChanOp` (DatToChop)**: Parses numeric columns/rows from a `DataTable` into a multi-channel `ChannelBuffer`.

---

## 3. Consequences
- **Positive**:
  - Full parity with modern real-time creative data flows.
  - Seamless Python integration for automation and procedural graph generation.
  - Low-latency real-time controller ingest for interactive projection mapping and gesture setups.
- **Negative / Mitigations**:
  - Large string tables (100,000+ rows) require virtual scrolling in the UI (`ImGuiListClipper`).
