# Project Plan: Phase 5 — Editor UI (Node Graph IDE)

## Executive Summary

Phase 5 delivers a high-performance, studio-grade visual programming IDE for **NodeForge**. Built entirely on Vulkan 1.3 and ImGui Docking, it provides an infinite node canvas with fluid pan/zoom, type-checked bezier connections, a family-categorized OP Create Palette (TAB menu), real-time parameter inspectors with expression editing toggles, live GPU texture preview viewports, a command pattern undo/redo stack, and an embedded Python console and timeline transport bar.

---

## 1. Aligned Technical Architecture (from `/grill-me`)

| Subsystem | Decision / Architecture | Rationale |
|-----------|------------------------|-----------|
| **Node Canvas** | Custom ImGui `ImDrawList` Canvas | Zero third-party baggage; custom multi-family pin layout, node headers, minimap, box-select, and adaptive grid lines. |
| **Workspace Layout** | Full ImGui Docking with Presets | Central Canvas, Right Parameter Inspector, Left/Top Live Viewer, Bottom Console & Python REPL, Bottom Timeline Bar. |
| **OP Create Palette** | Modal TAB / Double-Click Palette | Family tabs (TexOp, ChanOp, GeomOp, MatOp, DataOp, Comp), fuzzy typeahead search, keyboard arrow navigation. |
| **Texture Viewers** | Dual Live GPU Viewers | Main dedicated Viewer pane (RGBA channel isolation, aspect fit, zoom/pan) + toggleable live in-node mini-thumbnails. |
| **Undo/Redo System** | Command History Stack | Unlimited `Ctrl+Z`/`Ctrl+Y` stack capturing node create/delete, wire connect/disconnect, move, and param edits. |
| **Theme & Skin** | Neo Realms Dark Studio Theme | Clean-room charcoal/slate palette (`#18191c`, `#22242a`), glowing family accents (Amber Tex, Sky Chan, Emerald Geom). |

---

## 2. Component Breakdown & Source Layout

```text
src/ui/
├── EditorContext.h / .cpp       # Shared editor state (active graph, selection, hover, clipboard, undo)
├── EditorTheme.h / .cpp         # Neo Realms dark color palette, font setup, ImGui style config
├── UndoManager.h / .cpp         # Command pattern undo/redo history stack
├── commands/
│   ├── ICommand.h               # Base command interface (Execute, Undo, Redo, MergeWith)
│   ├── NodeCommands.h / .cpp    # CreateNodeCommand, DeleteNodeCommand, MoveNodesCommand
│   ├── WireCommands.h / .cpp    # ConnectWireCommand, DisconnectWireCommand
│   └── ParamCommands.h / .cpp   # SetParamValueCommand, SetParamExpressionCommand
├── canvas/
│   ├── NodeCanvas.h / .cpp      # Infinite canvas, pan/zoom, grid, selection, wire routing
│   ├── CanvasNode.h / .cpp      # Node box rendering, header, pins, live mini-thumbnail
│   └── CanvasWire.h / .cpp      # Cubic bezier curve rendering, type color coding, flow pulse
├── panels/
│   ├── ParameterPanel.h / .cpp  # Tabbed parameter inspector, sliders, color pickers, expr toggle
│   ├── ViewerPanel.h / .cpp     # Interactive GPU texture viewer (RGBA channels, pixel inspector)
│   ├── ConsolePanel.h / .cpp    # Spdlog sink viewer, log filters (info/warn/error), Python REPL
│   ├── TimelinePanel.h / .cpp   # Frame scrubber, FPS counter, Play/Pause/Loop transport bar
│   └── OpPaletteModal.h / .cpp  # TAB create menu with fuzzy search, family tabs, keyboard navigation
└── MainMenuBar.h / .cpp         # File (New/Open/Save/Exit), Edit (Undo/Redo/Cut/Copy/Paste), View layout presets
```

---

## 3. Detailed Implementation Steps

### Step 1: Editor Context, Theme, & Undo Manager (`src/ui/`)
1. Create `EditorContext` tracking `Graph*`, `std::unordered_set<NodeId> selectedNodes`, `NodeId activeNode`, `Pin* draggingPinWire`, `glm::vec2 canvasOffset`, `float canvasZoom`.
2. Create `EditorTheme` applying Neo Realms dark slate palette (`#18191c`, `#22242a`, `#2b2d35`, `#3e424e`) with crisp rounded frames and typography.
3. Implement `UndoManager` with `ExecuteCommand(cmd)`, `Undo()`, `Redo()`, and command coalescing for continuous drags and slider scrubs.

### Step 2: Custom Node Graph Canvas (`src/ui/canvas/`)
1. Implement infinite canvas math: Screen-to-Canvas and Canvas-to-Screen coordinate transformation with zoom range `0.2f` to `2.0f`.
2. Render background grid with adaptive minor (16px) and major (128px) grid lines that fade smoothly based on zoom factor.
3. Render nodes with family-colored headers (Amber TexOp, Sky Blue ChanOp, Emerald GeomOp, Indigo Comp), pin circles with type colors, and active selection glow borders.
4. Render cubic bezier curves connecting output pins to input pins with connection type validation and flow direction indicators.
5. Support multi-node selection rectangle (box select), dragging multiple nodes, copy/paste (`Ctrl+C`/`Ctrl+V`), and delete (`Delete`/`Backspace`).

### Step 3: OP Create Palette (TAB Menu) (`src/ui/panels/OpPaletteModal.h`)
1. Create modal popup triggered by `TAB` key or canvas double-click.
2. Provide family tabs: **ALL**, **TexOp**, **ChanOp**, **GeomOp**, **MatOp**, **DataOp**, **Comp**.
3. Implement fuzzy typeahead search filtering registered nodes from `NodeRegistry`.
4. Allow arrow key up/down navigation and `Enter` key to instantiate node at mouse cursor position with auto-selection.

### Step 4: Parameter Inspector Panel (`src/ui/panels/ParameterPanel.h`)
1. Display selected node's name, family, type, and comment.
2. Group parameters by pages (e.g. `Common`, `Settings`, `Color`, `Transform`).
3. Render interactive controls for float/int sliders, color pickers, vec2/vec3/vec4 inputs, booleans, and dropdowns.
4. Provide a toggle button beside each parameter to switch between **Constant Value** and **Python Expression** mode with live syntax feedback.

### Step 5: GPU Live Viewer Panel (`src/ui/panels/ViewerPanel.h`)
1. Integrate ImGui Vulkan texture descriptor set (`ImTextureID`) using `ImGui_ImplVulkan_AddTexture`.
2. Render live preview of the currently selected TexOp's output texture.
3. Add toolbar for zoom, 1:1 pixel view, fit-to-window, and RGBA channel isolation toggles (RGB, Red, Green, Blue, Alpha).
4. Add pixel inspector displaying normalized UV and exact color values under mouse cursor.

### Step 6: Console, Python REPL, & Timeline Bar (`src/ui/panels/`)
1. Create `ConsolePanel` with custom `spdlog` memory sink capturing log messages with level filtering and search.
2. Add interactive Python input bar executing commands directly against the active graph via `PythonEngine::ExecuteString()`.
3. Create `TimelinePanel` transport bar with Play/Pause/Step buttons, FPS readout (60 FPS), frame counter, and scrubber.

### Step 7: Main App Integration & Hotkeys (`src/app/main.cpp`)
1. Integrate ImGui Docking layout in `src/app/main.cpp` with default dock space split:
   - Central: Node Canvas
   - Right: Parameter Panel
   - Top-Left: Viewer Panel
   - Bottom: Console / Python REPL
   - Bottom Bar: Timeline Bar
2. Register global hotkeys:
   - `Ctrl+S`: Save Project
   - `Ctrl+O`: Open Project
   - `Ctrl+Z` / `Ctrl+Y`: Undo / Redo
   - `TAB`: Open OP Palette
   - `F`: Focus / Frame selected nodes
   - `Space`: Play / Pause timeline

---

## 4. Definition of Done (DoD) for Phase 5

- [ ] Complete ImGui Docking layout functioning seamlessly with GLFW Vulkan 1.3 backend.
- [ ] Custom Node Canvas with smooth 60fps pan/zoom, grid, multi-node box select, and bezier wire routing.
- [ ] OP Create Palette (TAB menu) with fuzzy search instantiating all 12 core operators.
- [ ] Parameter Inspector editing all param types (Float, Int, Bool, Vec2, Vec4, Color, String) and Python expressions.
- [ ] Live GPU Texture Viewer rendering TexOp outputs in real-time with RGBA isolation.
- [ ] Undo/Redo stack handling node creation, deletion, wiring, and parameter modifications.
- [ ] Zero memory leaks, zero validation layer errors, and 100% test pass rate.

---

## 5. Verification Plan

1. **Automated Unit Tests**:
   - `tests/unit/ui_undo_test.cpp`: Test `UndoManager` stack push, undo, redo, and coalescing.
   - `tests/unit/ui_canvas_math_test.cpp`: Test Screen-to-Canvas coordinate transforms and bounding box intersections.
2. **Interactive IDE Verification**:
   - Launch `nodeforge.exe` in interactive mode.
   - Press `TAB` -> spawn `ConstantTexOp` -> spawn `BlurTexOp` -> connect wire -> inspect live output in Viewer.
   - Tweak parameters in Parameter Panel -> verify real-time GPU texture update.
   - Press `Ctrl+Z` to undo wiring/parameters -> press `Ctrl+Y` to redo.
