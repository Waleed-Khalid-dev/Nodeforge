# Phase 8: DataOps + Script Nodes (DAT Pipeline) — Project Plan

## Executive Summary
Phase 8 implements the complete **DataOp (DAT) Pipeline** for NodeForge. It provides in-memory 2D tabular data manipulation (`DataTable`), raw/multiline text storage, Python script cooking and event execution (`ScriptDataOp`), JSON parsing and pointer queries (`JSONDataOp`), non-blocking asynchronous HTTP network requests (`WebDataOp`), low-latency UDP OSC In/Out messaging (`OSCInOp`, `OSCOutOp`), and seamless bidirectional bridges to ChanOp (`ChanToDataOp`, `DataToChanOp`).

---

## Architectural Decisions & Grill-Me Consensus

| Decision Branch | Chosen Architecture | Rationale & Specifications |
| :--- | :--- | :--- |
| **1. In-Memory Data Model** | **Unified `DataTable` Grid** | 2D cell matrix of UTF-8 strings, optional row/column header labels, CSV/TSV parsing/serialization, raw text storage as single-cell/line-oriented table, fast indexing via `table(row, col)` / `table['rowName', 'colName']` in C++ & Python. |
| **2. Python Scripting Model** | **Dual Cooking & Event Hooks** | `onCook(dat)` callback during DAG evaluation + `onPulse(dat, paramName)` / custom event triggers. Full exception isolation preventing host crashes with spdlog traceback reporting. |
| **3. Network & Async I/O** | **Thread-Safe Ingest Queue + Async HTTP** | Dedicated UDP background listener thread feeding lockless ring buffer for `OSCInOp` (drained into table on frame start), direct UDP socket send for `OSCOutOp`, and `std::jthread` async HTTP worker for `WebDataOp` with zero frame drops. |
| **4. Operator Roster** | **Full 11-Node Core Suite + Interop** | 11 operators: `TextDataOp`, `TableDataOp`, `ScriptDataOp`, `JSONDataOp`, `WebDataOp`, `OSCInOp`, `OSCOutOp`, `SelectDataOp`, `MergeDataOp`, `ChanToDataOp`, `DataToChanOp`. |
| **5. UI & Viewer Visualizer** | **Interactive Spreadsheet & Code Editor** | ImGui table with sortable resizable columns, virtual scrolling (10,000+ rows), inline cell editing, line-numbered monospace code view, and canvas node mini-table snippet previews. |

---

## Subphase Task Breakdown

### Subphase 8.1 — Core Data Architecture & ADR
- [ ] Create `docs/adr/ADR-0008-dataop-pipeline-and-scripting.md` documenting table memory layout, indexing, CSV/JSON serialization, thread-safe network ingest, and Python script hooks.
- [ ] Create `src/core/DataTable.h` and `src/core/DataTable.cpp`:
  - 2D grid matrix `std::vector<std::vector<std::string>>` with fast lookup by integer index or header string.
  - CSV / TSV string parsing, escaping, and formatting.
  - Raw multiline text load/save utility methods.
  - Cell accessors: `GetCell(row, col)`, `SetCell(row, col, value)`, `GetCellFloat()`, `GetCellInt()`.
  - Row & column insertion, deletion, resizing, clearing, and header management.
- [ ] Update `src/graph/PinValue.h` to support `DataTable` in `std::variant`.

### Subphase 8.2 — DataOp Base Class & Operator Specs
- [ ] Create 11 operator specs in `docs/operator-spec/`:
  - `TextDataOp.md`, `TableDataOp.md`, `ScriptDataOp.md`, `JSONDataOp.md`, `WebDataOp.md`, `OSCInOp.md`, `OSCOutOp.md`, `SelectDataOp.md`, `MergeDataOp.md`, `ChanToDataOp.md`, `DataToChanOp.md`.
- [ ] Implement `src/operators/data/DataOp.h` and `src/operators/data/DataOp.cpp`:
  - Base class inheriting from `Node` with `m_cachedTable`, `GetOutputTable()`, `GetInputTable(index)`, `SetOutputTable()`.
- [ ] Implement Generative & Storage DataOps:
  - `src/operators/data/TextDataOp.h` & `.cpp`: Multiline text editor, external file loading, file-change polling.
  - `src/operators/data/TableDataOp.h` & `.cpp`: CSV/TSV table generator and structured cell data store.

### Subphase 8.3 — Python Scripting, JSON & Filter DataOps
- [ ] Implement `src/operators/data/ScriptDataOp.h` & `.cpp`:
  - Executes embedded Python scripts with `onCook(dat)` and `onPulse(dat, paramName)`.
  - Exposes `dat.copy(inDat)`, `dat.appendRow()`, `dat.setCell()`, `dat.clear()`.
- [ ] Implement `src/operators/data/JSONDataOp.h` & `.cpp`:
  - Parses JSON strings/files using `nlohmann::json`.
  - JSON Pointer queries (e.g. `"/nodes/0/name"`), JSON formatting, JSON-to-Table key-value flattening.
- [ ] Implement Filter & Transformation DataOps:
  - `src/operators/data/SelectDataOp.h` & `.cpp`: Row/column range slicing, wildcard name matching (`col*`, `row*`), regex filtering.
  - `src/operators/data/MergeDataOp.h` & `.cpp`: Table concatenation by rows/columns, key-based joining.

### Subphase 8.4 — Network I/O & Interop Bridges
- [ ] Implement Network I/O DataOps:
  - `src/operators/data/OSCInOp.h` & `.cpp`: Multi-threaded UDP receiver parsing OSC packets (`/address val1 val2`) into timestamped message tables with lock-free ring buffer.
  - `src/operators/data/OSCOutOp.h` & `.cpp`: UDP OSC packet sender triggering on cook/change.
  - `src/operators/data/WebDataOp.h` & `.cpp`: Async HTTP GET/POST client storing status codes, response headers, and body text/JSON.
- [ ] Implement Bidirectional Interop Bridges:
  - `src/operators/data/ChanToDataOp.h` & `.cpp` (ChopToDat): Converts input `ChannelBuffer` channels into table columns with sample index rows.
  - `src/operators/data/DataToChanOp.h` & `.cpp` (DatToChop): Converts numeric table columns/rows into `ChannelBuffer` audio/motion channels.

### Subphase 8.5 — Python Module Exports, UI Viewer & Test Suite
- [ ] Update `src/python/PyNodeForge.cpp`:
  - Bind `DataTable` class (`row_count`, `col_count`, `cell(r, c)`, `append_row`, `to_string`, `from_csv`).
  - Add `node.table` / `node['row', 'col']` subscripts.
- [ ] Register all 11 DataOps in `src/graph/CoreNodes.cpp`.
- [ ] Update UI Panels:
  - `src/ui/panels/ViewerPanel.cpp`: Interactive spreadsheet grid with `ImGuiTableFlags_ScrollY | ImGuiTableFlags_Resizable | ImGuiTableFlags_Sortable`, cell editing, and line-numbered code viewer.
  - `src/ui/canvas/NodeCanvas.cpp`: Mini 3x3 table preview rendered on DataOp node bodies.
- [ ] Test Suite & Benchmarks in `tests/`:
  - `tests/unit/data_table_test.cpp`: CSV parsing, 2D indexing, row/col operations, JSON conversion.
  - `tests/unit/dataop_pipeline_test.cpp`: Test all 11 DataOps, Python script callbacks, JSON queries, and interop bridges.
  - `tests/benchmark/dataop_benchmark.cpp`: High-throughput table processing (>500,000 cells/sec) and zero memory leaks.
- [ ] Update `CMakeLists.txt` and `tests/CMakeLists.txt`.
- [ ] Build and verify all 65+ unit and benchmark tests passing.
- [ ] Update `STATUS.md` and `.agent/memory/MEMORY.md`.

---

## Verification Plan

### Automated Unit & Benchmark Tests
1. `DataTableTest`: Verification of cell operations, row/col insertions, CSV escaping, and memory safety.
2. `DataOpPipelineTest`: Verification of Text, Table, Script, JSON, Select, Merge, OSC, Web, and ChanToData/DataToChan bridges.
3. `DataOpBenchmark`: 10,000-frame cook loop with zero memory leaks and >500k cells/sec transformation throughput.

### Manual / Visual Verification
1. Launch `nodeforge.exe`.
2. Create `TableDataOp`, add rows/columns, inspect in ViewerPanel spreadsheet.
3. Create `ScriptDataOp` mutating table values dynamically on frame cook.
4. Verify `ChanToDataOp` converting LFO channel output to real-time tabular data.
