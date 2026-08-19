# Phase 6 — Project System & Components (.nfp / .nfc / Subnetworks)

**Owner:** Waleed Khalid  
**Company:** Neo Realms  
**Branch:** `main`  
**Status:** In Planning  
**Target:** 35/35 existing tests passing + Phase 6 Project & Component test suite passing

---

## 1. Overview & Objectives

Phase 6 implements the complete project lifecycle, persistent serialization, subnetwork encapsulation, and component reuse system for NodeForge:
1. **`.nfp` Project Serialization (JSON v1)**: Complete graph state, node positions, canvas zoom/pan, timeline settings, parameter values & Python expressions, and recursive subgraphs.
2. **`.nfc` Component System**: Export and import reusable modular subnetworks across projects.
3. **`ContainerComp` & Hierarchical Subgraphs**: Enter into container nodes, navigate with an interactive breadcrumb address bar (`/project1/container1`), and drill up/down seamlessly.
4. **Boundary Routing (`InOp` & `OutOp`)**: Subgraph input/output proxy operators that dynamically create and route pins on parent `ContainerComp` nodes.
5. **Parameter Promotion**: Alias and expose internal node parameters directly onto the outer container's parameter interface.
6. **Autosave & Crash Recovery**: Periodic background autosave (`.nfp.autosave`) and startup ungraceful exit detection with one-click restore.
7. **Recent Projects & File Menu Integration**: Persistent history of recent projects in user config with dirty-state window title tracking.

---

## 2. Architecture & File Layout

```
src/
├── project/
│   ├── ProjectFile.h / .cpp           # Project metadata, schema versioning, save/load coordinator
│   ├── ProjectSerializer.h / .cpp     # Graph & UI state serialization to/from JSON
│   ├── ComponentSerializer.h / .cpp   # .nfc component export/import
│   ├── AutosaveManager.h / .cpp       # Background timer, dirty tracking, .nfp.autosave
│   ├── RecentProjectsManager.h / .cpp # Recent file list persistence in user appdata
│   └── PathUtils.h / .cpp             # Project-relative vs absolute path resolution
├── operators/
│   └── comp/
│       ├── ContainerComp.h / .cpp     # Container operator holding an inner Graph
│       ├── InOp.h / .cpp              # Subgraph input proxy operator
│       └── OutOp.h / .cpp             # Subgraph output proxy operator
└── ui/
    ├── canvas/
    │   └── BreadcrumbBar.h / .cpp     # Canvas top breadcrumb bar (/project1/container1)
    └── dialogs/
        └── FileDialogUtils.h / .cpp   # Native / portable open & save dialog helpers
```

---

## 3. Tasks Breakdown

### Task 6.1 — Project File Serialization (`.nfp` JSON v1)
- [ ] Implement `ProjectSerializer`:
  - `SaveProject(const std::string& filepath, const ProjectData& data) -> bool`
  - `LoadProject(const std::string& filepath, ProjectData& outData) -> bool`
- [ ] Serialize full graph topology, node IDs, types, names, UI positions, custom parameter values/modes/expressions, and wires.
- [ ] Serialize canvas view state (`pan`, `zoom`) and timeline configuration (`fps`, `startFrame`, `endFrame`, `currentFrame`, `bpm`).
- [ ] Schema versioning (`"schema_version": 1`) with forward-compatibility validation.

### Task 6.2 — Path Resolution & Media Asset Management
- [ ] Implement `PathUtils`:
  - `ToProjectRelative(const std::string& absolutePath, const std::string& projectDir) -> std::string`
  - `ToAbsolute(const std::string& relativePath, const std::string& projectDir) -> std::string`
- [ ] Ensure all filepaths in parameters (e.g. `LoadImageTexOp` image paths) serialize as project-relative paths.

### Task 6.3 — Container Component (`ContainerComp`) & Hierarchy Runtime
- [ ] Create `ContainerComp` subclass of `Node` (Family: `NodeFamily::Comp`).
- [ ] Holds an inner `Graph` instance (`std::unique_ptr<Graph> m_innerGraph`).
- [ ] Graph evaluation propagates into and through the inner graph during `Cook()`.
- [ ] Support nested hierarchy: `Graph` can contain `ContainerComp`, which contains another `Graph`.

### Task 6.4 — Boundary Routing Operators (`InOp` & `OutOp`)
- [ ] Implement `InOp` (Input proxy operator):
  - Declares an output pin inside the subnetwork and dynamically provisions an input pin on the parent `ContainerComp`.
- [ ] Implement `OutOp` (Output proxy operator):
  - Declares an input pin inside the subnetwork and dynamically provisions an output pin on the parent `ContainerComp`.
- [ ] Dynamic pin synchronization when `InOp`/`OutOp` nodes are added, renamed, or deleted inside the container.

### Task 6.5 — UI Subnetwork Navigation & Breadcrumbs
- [ ] Add `BreadcrumbBar` at top of `NodeCanvas`.
- [ ] Visual path display: `/` (Root) -> `/container1` -> `/container1/fx_sub`.
- [ ] Double-click on `ContainerComp` in canvas enters the container network.
- [ ] Click breadcrumb element or press `U` / `Backspace` to navigate up one level.
- [ ] `EditorContext` tracks current active `Graph*` and breadcrumb hierarchy stack.

### Task 6.6 — Component Export & Import (`.nfc`)
- [ ] Implement `ComponentSerializer`:
  - `ExportComponent(const std::string& filepath, ContainerComp* comp) -> bool`
  - `ImportComponent(const std::string& filepath, Graph* targetGraph, glm::vec2 spawnPos) -> ContainerComp*`
- [ ] Self-contained `.nfc` JSON schema containing subnetwork nodes, wires, promoted parameters, and local canvas positions.

### Task 6.7 — Autosave & Crash Recovery System
- [ ] Implement `AutosaveManager`:
  - 60-second periodic background timer.
  - Automatically triggers if `EditorContext::IsGraphDirty()` is true.
  - Writes to `<project_path>.nfp.autosave` (or `<appdata>/autosave/<timestamp>.nfp` for untitled projects).
- [ ] Startup Crash Recovery check:
  - If an `.nfp.autosave` exists with a timestamp newer than the main `.nfp`, display an ImGui modal prompt to recover or discard the autosave.

### Task 6.8 — Recent Files & Main Menu Actions
- [ ] Implement `RecentProjectsManager` persisting top 10 recent project paths to user config.
- [ ] Update `MainMenuBar` with full working callbacks:
  - `File -> New Project` (prompts to save if dirty, resets graph to default starter template)
  - `File -> Open Project...`
  - `File -> Save Project` / `Save Project As...`
  - `File -> Export Component...` / `Import Component...`
  - `File -> Recent Projects -> [list]`
- [ ] Dynamic window title: `NodeForge - [ProjectName]*` showing modified dirty indicator.

---

## 4. Verification & Testing Plan

### Automated Unit Tests (`tests/unit/project_serialization_test.cpp` & `tests/unit/component_hierarchy_test.cpp`):
1. `ProjectSerializationTest.SaveAndLoadFullGraphWithUI`: Verify 100% roundtrip fidelity of nodes, params, expressions, wires, and canvas pan/zoom.
2. `ProjectSerializationTest.RelativePathResolution`: Verify asset paths correctly convert between relative and absolute representations.
3. `ProjectSerializationTest.AutosaveAndCrashRecovery`: Verify autosave generation when dirty and recovery restoration.
4. `ComponentHierarchyTest.ContainerSubnetworkCook`: Verify cook propagation through `InOp -> Subnetwork Operators -> OutOp`.
5. `ComponentHierarchyTest.DynamicBoundaryPinSync`: Verify adding `InOp`/`OutOp` inside container updates parent pins immediately.
6. `ComponentHierarchyTest.ComponentExportAndImport`: Verify `.nfc` export from one graph and import into another graph.

---

## 5. Definition of Done (DoD)

- [ ] `.nfp` project save and load reproduces exact cook output, node network, parameter modes, and UI viewport.
- [ ] `.nfc` components can be exported from a project and imported into a different project cleanly.
- [ ] Hierarchical navigation in `NodeCanvas` allows seamless drill-down into containers and back out via breadcrumbs.
- [ ] Autosave runs in the background and crash recovery restores uncommitted work.
- [ ] All existing 35 unit & benchmark tests + new Phase 6 test suites pass with 0 failures.
- [ ] `STATUS.md` updated and atomic commits pushed to GitHub `main`.
