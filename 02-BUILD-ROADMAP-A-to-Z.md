# TD-Class Real-Time Media Platform — Build Roadmap A → Z

> **Product codename (working):** `NodeForge` (rename anytime — do **not** use “TouchDesigner”, “Derivative”, or TD trademarks in product name/UI).  
> **Goal:** Private company platform in the **same product category** as TouchDesigner: real-time node graph, GPU textures, channels, geometry, data, components, Python, I/O, multi-output.  
> **Method:** Clean-room, original UI, original operator names; public concepts only.  
> **This file is the single source of truth** for humans and coding agents.  
> **Last updated:** 2026-08-04

---

## 0. How to Use This Document (Agents & Humans)

### 0.1 Absolute rules for coding agents

1. **Read this file + `01-TouchDesigner-Research-And-Feasibility.md` before any implementation.**  
2. **Implement only the current phase** listed in `STATUS.md` (create it in Phase 0). Never jump phases.  
3. **Do not invent operators, file formats, or APIs** that contradict this roadmap. If missing, extend this roadmap first.  
4. **Do not copy TouchDesigner UI, icons, assets, operator spellings as trademarks, or decompile TD.**  
5. **Every phase ends with a Definition of Done checklist.** No checklist pass → phase incomplete.  
6. **Prefer boring, testable cores** over flashy demos that skip cook correctness.  
7. **One architecture owner:** graph runtime + data model changes require explicit design notes in `/docs/adr/`.  
8. **No drive-by refactors** outside the task.  
9. When uncertain: **stop and document a question** in `/docs/OPEN-QUESTIONS.md` rather than hallucinating.  
10. All public identifiers use **NodeForge** naming (`nf::`, `NF_`, `NodeForge`).

### 0.2 Recommended agent work units

| Agent role | Allowed to touch | Forbidden |
|------------|------------------|-----------|
| **Architect** | ADRs, this roadmap updates, interface headers | Random feature code without phase |
| **Runtime** | Graph, cook, scheduler, data types | UI skinning, device SDKs |
| **GPU** | Vulkan backend, textures, shaders, render graph | Python bindings redesign |
| **Operators** | One operator family at a time per task | New families without phase unlock |
| **UI** | Editor, parameter panels, viewers | Runtime cook semantics |
| **Scripting** | Python embed, expressions | GPU driver code |
| **QA** | Tests, fixtures, perf harness | Shipping untested ops |

### 0.3 Definition of “same thing as TouchDesigner” for this project

**Same category capabilities**, not a clone:

| TD concept | NodeForge equivalent name |
|------------|---------------------------|
| Operator | **Node** (typed) |
| TOP | **TexOp** (texture op) |
| CHOP | **ChanOp** (channel op) |
| SOP | **GeomOp** (geometry op) |
| MAT | **MatOp** (material) |
| DAT | **DataOp** (table/text/script) |
| COMP | **Comp** (component / container / scene / panel) |
| Network | **Graph** |
| Cook | **Evaluate / cook** |
| Parameter | **Param** |
| `.toe` / `.tox` | **`.nfp` project / `.nfc` component** (names finalizable in Phase 0) |

---

## 1. Product Vision & Non-Goals

### 1.1 Vision

A private Windows-first application where artists/engineers:

1. Place nodes in a graph  
2. Wire typed outputs → inputs  
3. See live viewers (texture, channels, geometry, table)  
4. Drive params with numbers, expressions, and Python  
5. Output real-time visuals to windows and network video  
6. Save/load projects and reusable components  
7. Extend with plugins  

### 1.2 Year-1 north star (must ship)

**One end-to-end company workflow**, example:

> Camera or video file → TexOps effects → mix with generative pattern → ChanOps animate params → Window output at stable 60 FPS (720p/1080p) → save/load project → Python can set a param and create a simple node.

### 1.3 Non-goals (Year 1)

- Full operator count parity with TD  
- macOS/Linux production support  
- Marketplace / public distribution  
- Pixel-identical TD UI  
- Every protocol (DMX, Kinect, etc.) on day one  
- Offline film VFX feature set  

---

## 2. Recommended Tech Stack (Aligned With TD or Better)

### 2.1 Stack decision matrix

| Layer | TouchDesigner-class (public) | **NodeForge choice (2026)** | Why |
|-------|------------------------------|-----------------------------|-----|
| Systems language | C++ | **C++20/23** | Real-time control, GPU, plugins |
| Build | MSVC projects | **CMake 3.28+ + Ninja** + **vcpkg** | Reproducible multi-agent builds |
| Primary GPU API | OpenGL + GLSL | **Vulkan 1.3** (+ optional GL interop later) | Modern, explicit, better multi-queue |
| Vulkan bootstrap | — | **vk-bootstrap** ✅ NEW | Reduces 800-line Vulkan init boilerplate to ~50 lines |
| Shader languages | GLSL | **GLSL → SPIR-V** via **shaderc/glslang**; optional **Slang** later | TD familiarity + modern IR |
| Memory GPU | — | **Vulkan Memory Allocator (VMA)** | Production GPU alloc |
| Window / input | OS | **GLFW 3.4+** or **SDL3** | Simple; swap later if needed |
| Tool UI | Custom | **Dear ImGui (docking + multi-viewport)** + **imgui-node-editor** | Fast iteration for node tools |
| UI file dialogs | — | **nativefiledialog-extended** ✅ NEW | Native OS file picker inside ImGui |
| UI icons | — | **IconFontCppHeaders (Font Awesome)** ✅ NEW | Free icon set rendered as ImGui text |
| App shell (optional later) | — | **Qt 6** only if product needs native chrome | Phase 6+ optional |
| Scripting | Python | **CPython 3.11 embed** (confirmed) + **pybind11** | 3.11 chosen for stable embed ABI |
| Math | — | **glm** | GLSL-like math |
| Containers / utils | — | **spdlog**, **fmt**, **nlohmann/json**, **stb** | Standard |
| Image/video decode | various | **FFmpeg** (track LGPL/GPL), **stb_image** for stills | Practical |
| HDR image support | — | **tinyexr** ✅ NEW | EXR format for professional content |
| 3D model loading | — | **assimp** ✅ NEW | Loads FBX/OBJ/GLTF — no custom parsers |
| Computer vision / warp calibration | — | **OpenCV** (via vcpkg) ✅ NEW | Homography + projector auto-calibration for Phase 10b |
| Gesture / depth camera | — | **librealsense2** (Intel RealSense SDK) ✅ NEW | Best active C++ depth SDK 2026 |
| Gesture AI tracking | — | **MediaPipe C++ API** ✅ NEW | GPU-accelerated hand/body landmark tracking |
| Audio | CHOP-side | **miniaudio** or **PortAudio** first | Simple; expand later |
| MIDI | — | **RtMidi** | Already in roadmap |
| Networking protocols | OSC/MIDI/NDI… | **oscpack** or **liblo**, **RtMidi**, **NDI SDK v6.3.2** ✅ CONFIRMED | NDI registration complete |
| IPC GPU share (Win) | Spout | **Spout2** library | Windows live share |
| Capture cards | — | **Blackmagic DeckLink SDK** ✅ NEW (Year 1, optional) | Free download from blackmagicdesign.com/developer |
| Testing | — | **GoogleTest** + **Google Benchmark** ✅ NEW | Cook correctness + perf benchmarks |
| Profiling | — | **Tracy** | Frame + cook profiling |
| CI | — | **GitHub Actions** Windows runners (github.com/Waleed-Khalid-dev/Nodeforge) ✅ CONFIRMED | Gate main branch |


### 2.2 Why not pure OpenGL-only?

TD’s classic custom TOP path is OpenGL-centric. For a **new** engine in 2026:

- **Vulkan primary** is the better long-term bet (explicit sync, compute, multi-GPU readiness).  
- Keep **GLSL authoring** so technical artists feel at home.  
- If a specific library forces GL, add **GL interop** in a later phase — do not derail core.

### 2.3 Why not Electron / web UI for core?

Real-time node tools need:

- Sub-frame UI + GPU texture viewers  
- Deterministic cook loops  
- Plugin ABIs in C++

Web UIs are fine for **remote control panels later**, not the core editor runtime.

### 2.4 Language split

| Language | Use |
|----------|-----|
| **C++** | Engine, GPU, cook, operators, UI host |
| **GLSL** | TexOp / MatOp shaders |
| **Python** | Expressions, scripting, automation, company tools |
| **JSON** | Project serialization v1 |
| **Markdown** | Docs / ADRs only |

**Do not** introduce Rust/C#/Java for core in v1 (extra agent confusion). Optional Rust plugins only after ABI is stable.

### 2.5 Target platforms

| Phase | Platform |
|-------|----------|
| 0–5 | **Windows 10/11 x64** only |
| 6+ | macOS exploration (Metal via MoltenVK or native later) |
| Later | Linux |

### 2.6 Hardware baseline (dev + QA)

- NVIDIA RTX 3060 12 GB or better (primary — **confirmed minimum for 5760×1080 projection output**)  
- One AMD GPU machine in matrix by Phase 4  
- 32 GB RAM recommended  
- Multi-monitor required by Phase 10b (projection mapping)  
- Intel RealSense D455 depth camera for gesture work (Phase 11)  

---

### 2.7 Approved Third-Party Modules (Do Not Reinvent)

> **Rule for agents:** Before writing any utility code, check if this list already covers it. Every library here was explicitly approved to avoid building it from scratch.

| Library | via vcpkg? | What it replaces | Phase first used | Weeks saved |
|---------|-----------|-----------------|-----------------|-------------|
| **vk-bootstrap** | ✅ yes | 800 lines of Vulkan instance/device/swapchain init | 1 | 2–3 |
| **VMA** | ✅ yes | Custom GPU allocator | 1 | 1–2 |
| **GLFW 3.4** | ✅ yes | OS window + input system | 1 | 1 |
| **glm** | ✅ yes | Math library | 1 | — |
| **imgui-node-editor** | ✅ yes | Entire node canvas UI (pan/zoom/wires/select) | 2 | 4–8 |
| **Dear ImGui (docking)** | ✅ yes | Full editor UI shell | 2 | 6–10 |
| **nativefiledialog-extended** | ✅ yes | OS file picker dialogs | 5 | 1 |
| **IconFontCppHeaders** | manual | Icon glyphs inside ImGui | 5 | 1 |
| **pybind11** | ✅ yes | Python ↔ C++ bindings | 3 | 4–6 |
| **shaderc / glslang** | ✅ yes | GLSL → SPIR-V shader compilation | 4 | 2–3 |
| **stb_image** | ✅ yes | PNG/JPG/BMP image loading | 4 | — |
| **tinyexr** | ✅ yes | EXR HDR image loading | 4 | 1 |
| **FFmpeg** | ✅ yes | Video file decode + encode | 4, 10 | 4–8 |
| **spdlog** | ✅ yes | Logging | 0 | — |
| **fmt** | ✅ yes | String formatting | 0 | — |
| **nlohmann/json** | ✅ yes | JSON project serialization | 2 | — |
| **assimp** | ✅ yes | 3D model loading (FBX/OBJ/GLTF) | 9 | 2–3 |
| **OpenCV** | ✅ yes | Projector homography + auto-calibration | 10b | 3–5 |
| **librealsense2** | manual | Intel RealSense depth camera SDK | 11 | 4–6 |
| **MediaPipe C++ API** | manual | Hand/body gesture AI tracking | 11 | 6–10 |
| **miniaudio** | ✅ yes | Audio device I/O | 7 | 2–3 |
| **RtMidi** | ✅ yes | MIDI device I/O | 11 | 1–2 |
| **oscpack / liblo** | ✅ yes | OSC protocol | 8, 11 | 1 |
| **NDI SDK v6.3.2** | manual | Network video I/O | 10 | 2–3 |
| **Spout2** | manual | GPU texture sharing Windows | 10 | 2 |
| **Blackmagic DeckLink SDK** | manual | Capture card I/O (optional Year 1) | 10 | 2–3 |
| **GoogleTest** | ✅ yes | Unit + cook integration tests | 2 | — |
| **Google Benchmark** | ✅ yes | Perf benchmarks for cook/GPU | 12 | — |
| **Tracy** | ✅ yes | Frame + cook profiling overlay | 1 | — |

**Manual = download separately, not in vcpkg registry (or requires SDK registration).**

#### What you CANNOT avoid building yourself

| Component | Why no library solves it |
|-----------|------------------------|
| Cook / dirty propagation engine | Core of NodeForge — unique to your graph model |
| Parameter system + expressions | Tied to your node type system |
| Project save/load (`.nfp` / `.nfc`) | Your own schema |
| Node type registry | Your operator family system |
| Warp shader (Vulkan GLSL) | Port algorithm from ofxWarpBlend — ~200 lines GLSL |
| Softedge blend mask shader | Simple gradient math — ~50 lines GLSL |

## 3. Repository Structure (Create in Phase 0)

```text
NodeForge/
├── README.md
├── STATUS.md                 # current phase + owners
├── AGENTS.md                 # short agent rules (pointer to this roadmap)
├── CMakeLists.txt
├── vcpkg.json
├── docs/
│   ├── 01-TouchDesigner-Research-And-Feasibility.md  # (this repo copies)
│   ├── 02-BUILD-ROADMAP-A-to-Z.md                    # this file
│   ├── OPEN-QUESTIONS.md
│   ├── adr/                  # Architecture Decision Records
│   ├── operator-spec/        # one MD per operator
│   └── api/                  # public C++ / Python API notes
├── third_party/              # sparingly; prefer vcpkg
├── cmake/
├── src/
│   ├── app/                  # main executable
│   ├── core/                 # types, result, ids, time
│   ├── graph/                # nodes, wires, dirty, cook
│   ├── params/               # parameter system + expressions
│   ├── gpu/                  # vulkan device, frames, textures
│   ├── render/               # pass graph, viewers GPU side
│   ├── ops/
│   │   ├── tex/              # TexOps
│   │   ├── chan/             # ChanOps
│   │   ├── geom/             # GeomOps
│   │   ├── mat/              # MatOps
│   │   ├── data/             # DataOps
│   │   └── comp/             # Comps
│   ├── script/               # Python embed
│   ├── io/                   # devices, protocols
│   ├── project/              # load/save .nfp .nfc
│   ├── ui/                   # ImGui editor
│   └── plugin/               # plugin host ABI
├── shaders/
├── python/
│   └── nodeforge/            # shipped Python package surface
├── tests/
│   ├── unit/
│   ├── cook/
│   └── perf/
├── samples/                  # example graphs
├── tools/                    # codegen, op registration helpers
└── assets/                   # original icons (company-made)
```

---

## 4. Core Architecture (Must Implement Before Operator Flood)

### 4.1 Runtime loop (target)

```text
while running:
  1. poll input / UI
  2. advance clock (frame time, timeline)
  3. mark dirty from UI/param/script/IO events
  4. cook graph for required outputs (viewers + window outs)
  5. GPU submit + present
  6. profile / stats
```

### 4.2 Identity model

```text
NodeId, PinId, WireId, ParamId, GraphId, ComponentId  — strong typed IDs
```

- Never use raw strings as primary identity in hot paths.  
- Stable IDs in project files (UUID or monotonic + map).

### 4.3 Type system (pins)

Minimal v1 pin types:

| PinType | Payload |
|---------|---------|
| `Texture` | `gpu::TextureHandle` + format/size/meta |
| `Channels` | `ChanBuffer` (names, rate, samples, time slice) |
| `Geometry` | `GeomMesh` (CPU first; GPU buffers later) |
| `Table` | `DataTable` |
| `String` | text |
| `Float` / `Int` / `Bool` | scalar (for simple links) |
| `Any` | discouraged in v1; use converters |

**Converters** are explicit nodes (e.g. Chan → Param binding is a system feature, not silent magic everywhere).

### 4.4 Cook algorithm (v1 — required)

**Pull model with dirty flags:**

1. Outputs declare demand (Window, Viewer, Mark).  
2. Traverse dependencies backward; collect subgraph.  
3. Topo-sort.  
4. Cook nodes in order if dirty or time-dependent.  
5. Cache outputs; clear dirty.  

**Must support:**

- Bypass  
- Lock/cook always (time-slicing nodes)  
- Error state on node without crashing app  
- Cook budget / partial cook later (Phase 4+)  

### 4.5 Parameter system

Each node has a **ParamPage** list:

- name, label, type (float, int, bool, menu, file, RGB, pulse…)  
- min/max/default  
- expression string (optional)  
- bind target (optional ChanOp channel)  

Expression languages v1:

1. Numeric literal  
2. Simple ref: `op('noise1').par.speed` style **or** cleaner `nodes["noise1"].params.speed`  
3. Python expression mode (Phase 3)

### 4.6 Component model

- A **Comp** contains a child **Graph**.  
- External inputs/outputs (promoted pins).  
- Save as `.nfc`.  
- Clone instance (by reference or deep copy — decide in ADR; v1 deep copy OK).

### 4.7 Threading model (v1)

| Thread | Work |
|--------|------|
| Main | UI, present, orchestration |
| Cook | Optional later; v1 can cook on main if budgeted |
| Python | Same as cook initially; isolate in Phase 3.5 |
| IO | Device readers push into thread-safe queues |

**Rule:** GPU work recorded in defined frames; no random GL/Vulkan calls from Python threads in v1.

---

## 5. Phased Roadmap (Execute In Order)

> **Agents:** Only implement tasks under the active phase.  
> Update `STATUS.md` when a phase DoD is met.

---

### Phase 0 — Project Genesis & Contracts  
**Duration:** 1–2 weeks  
**Goal:** Empty app that builds; docs; rules; no features yet.

#### Tasks

0.1 Create repo structure (Section 3).  
0.2 CMake + vcpkg skeleton; Hello window (GLFW).  
0.3 Copy research + roadmap into `docs/`.  
0.4 Write `AGENTS.md` (short) + `STATUS.md` (`phase: 0`).  
0.5 ADR-0001: stack confirmation (Vulkan, ImGui, Python later).  
0.6 ADR-0002: file formats `.nfp` / `.nfc`.  
0.7 ADR-0003: naming (TexOp/ChanOp/…).  
0.8 Coding standards: clang-format, `.editorconfig`, warnings-as-errors on core.  
0.9 CI: configure + build empty app on Windows.  
0.10 Legal hygiene note: no TD assets; original icons only.

#### Definition of Done

- [x] `cmake --build` succeeds on clean Windows machine  
- [x] Window opens and closes cleanly  
- [x] Docs present; STATUS.md exists  
- [x] No operator code yet (except stubs if needed)  

#### Deliverables

- Runnable empty shell  
- Doc set  
- CI green  

---

### Phase 1 — GPU Foundation  
**Duration:** 3–6 weeks  
**Goal:** Vulkan device + swapchain + clear + texture upload/display.

#### Tasks

1.1 Vulkan instance/device/queues (graphics + present; compute optional).  
1.2 Swapchain resize handling.  
1.3 VMA integration.  
1.4 `Texture2D` abstraction (R8G8B8A8_UNORM, R16G16B16A16_SFLOAT later).  
1.5 Fullscreen triangle + sample texture shader.  
1.6 CPU image load (png) → upload → present.  
1.7 Frame resources (per-frame command buffers, fences, semaphores).  
1.8 Debug: validation layers in Debug builds.  
1.9 Tracy markers around frame.  
1.10 Unit test: device creation smoke test (optional headless skip).  

#### Definition of Done

- [ ] App shows a loaded image in a Vulkan window  
- [ ] Resize does not crash  
- [ ] Validation layers clean on happy path  
- [ ] Documented `gpu::Device` / `gpu::Texture` API in `docs/api/`  

#### Forbidden this phase

- Node graph UI  
- Python  
- Video decode  

---

### Phase 2 — Graph Runtime (Heart of the Product)  
**Duration:** 4–8 weeks  
**Goal:** In-memory graph with cook — still minimal UI OK (even console/test harness).

#### Tasks

2.1 `Node`, `Pin`, `Wire`, `Graph` classes.  
2.2 Registration registry: `NodeTypeInfo` (name, pins, factory).  
2.3 Dirty propagation.  
2.4 Topo sort + cycle detection (reject cycles with error).  
2.5 Cook context: time, delta, frame index.  
2.6 Output cache per pin.  
2.7 Error/status per node.  
2.8 Test operators (CPU-only):  
    - `Chan.Const`  
    - `Chan.MathAdd`  
    - `Data.TableConst`  
2.9 GoogleTest cook graph tests (10+ cases).  
2.10 Serialization sketch: graph to JSON roundtrip (no full project UI).  

#### Definition of Done

- [ ] Tests prove cook order + dirty + cache  
- [ ] Cycle graphs fail safely  
- [ ] JSON roundtrip for simple graph  
- [ ] ADR-0004: cook model  

#### Agent note

**Do not** add 50 operators here. Only enough to test the runtime.

---

### Phase 3 — Parameter System + Expressions + Python Embed  
**Duration:** 4–7 weeks  
**Goal:** TD-like superpower: drive anything from script/expressions.

#### Tasks

3.1 Param types + storage + undo-ready value model (undo can be stub).  
3.2 Expression parser v1 (literals + param refs).  
3.3 Embed CPython; bootstrap `nodeforge` module.  
3.4 pybind11:  
    - list nodes  
    - get/set param  
    - create node  
    - connect pins  
3.5 Execute Python from a DataOp `Script` (// Phase 3 can be API-only; op in 3b).  
3.6 GIL policy documented.  
3.7 Sandbox note: private tool → trust company scripts; still catch exceptions.  
3.8 Tests: set param from Python; create node from Python.  

#### Definition of Done

- [ ] Python REPL or script file can mutate graph safely  
- [ ] Expression on a float param evaluates each cook when dirty  
- [ ] Crashes in Python do not bring down process without log  

---

### Phase 4 — TexOp Pipeline (GPU Nodes)  
**Duration:** 6–10 weeks  
**Goal:** Real texture operators cooking on GPU.

#### Minimum TexOp set (implement in this order)

| # | Node | Behavior |
|---|------|----------|
| 1 | `Tex.Null` | Pass-through |
| 2 | `Tex.Constant` | Solid color |
| 3 | `Tex.Noise` | Simple value noise / FBM |
| 4 | `Tex.LoadImage` | File still image |
| 5 | `Tex.Transform` | Crop/tile/scale UV |
| 6 | `Tex.Composite` | Over/add/multiply dual input |
| 7 | `Tex.Blur` | Separable Gaussian |
| 8 | `Tex.Level` | Brightness/contrast/gamma |
| 9 | `Tex.Resolution` | Force size |
| 10 | `Tex.ToWindow` | Present graph output (or Comp.Window) |

#### Tasks

4.1 TexOp base class: declare output format, cook GPU pass.  
4.2 Render target pool / texture cache.  
4.3 Shader hot-reload (dev).  
4.4 Viewer texture → ImGui image (if UI started) or debug window.  
4.5 Performance: no full readback on cook path.  
4.6 Tests: CPU reference for Constant/Level where feasible; smoke GPU tests.  

#### Definition of Done

- [ ] Graph: Noise → Blur → Level → Window runs at real-time on mid GPU  
- [ ] Changing a param re-cooks only dependents  
- [ ] 1080p60 achievable on RTX-class for this simple graph  

---

### Phase 5 — Editor UI (Node Graph IDE)  
**Duration:** 6–10 weeks  
**Goal:** Usable visual programming UI (original design).

#### Tasks

5.1 ImGui docking layout: Graph | Params | Viewer | Console | Timeline stub.  
5.2 Node canvas: pan/zoom, select, box select.  
5.3 Create node palette (search).  
5.4 Connect pins with type check colors.  
5.5 Parameter panel for selection.  
5.6 Viewer pane for Texture / Channels / Table.  
5.7 Context menus: delete, bypass, rename.  
5.8 Undo/redo stack (commands).  
5.9 Keybinds document.  
5.10 Original theme (dark tool UI — **not** TD skin copy).  
5.11 Performance: do not rebuild whole ImGui font atlas every frame carelessly.  

#### UI design principles (agent-facing)

- Clarity > skeuomorphism  
- Type-colored pins  
- Always-visible cook error badge  
- Viewer framerate independent settings  
- No trademarked TD layout cloning requirement  

#### Definition of Done

- [ ] Artist can build Noise→Blur→Window without typing code  
- [ ] Save is not required yet, but create/connect/delete is solid  
- [ ] UI stays usable with 100+ nodes (basic)  

---

### Phase 6 — Project System & Components  
**Duration:** 3–6 weeks  

#### Tasks

6.1 `.nfp` project save/load (JSON v1 → binary later optional).  
6.2 Autosave + crash recovery draft.  
6.3 `.nfc` component export/import.  
6.4 Path handling for media assets (relative paths).  
6.5 Graph hierarchy: enter/exit Comp.  
6.6 Promote params to Comp external.  
6.7 Recent files list.  

#### Definition of Done

- [ ] Create graph → save → quit → load identical cook result  
- [ ] Component reused in two places  

---

### Phase 7 — ChanOps (Control / Animation / Audio-lite)  
**Duration:** 5–8 weeks  

#### Minimum ChanOp set

| Node | Behavior |
|------|----------|
| `Chan.Const` | Constant channels |
| `Chan.Time` | Timeline / frame / seconds |
| `Chan.LFO` | Sine/tri/saw/square |
| `Chan.Noise` | Channel noise |
| `Chan.Math` | + - * / clamp remap |
| `Chan.Filter` | Lag / FIR simple |
| `Chan.Merge` | Merge channel sets |
| `Chan.Select` | Select channels |
| `Chan.Trail` | History buffer (debug) |
| `Chan.AudioFile` | Optional early audio amplitude |

#### System features

7.1 Bind Chan channel → Param (core binding).  
7.2 Time slicing policy documented.  
7.3 Channel viewer (scope).  
7.4 Tests for math/filter.  

#### Definition of Done

- [ ] LFO binds to `Tex.Noise` strength visually  
- [ ] Channel scope viewer works  

---

### Phase 8 — DataOps + Script Nodes  
**Duration:** 3–5 weeks  

#### Minimum DataOp set

| Node | Behavior |
|------|----------|
| `Data.Text` | Text storage |
| `Data.Table` | Table edit |
| `Data.Script` | Run Python on cook/pulse |
| `Data.JSON` | Parse/pointer get |
| `Data.Web` (optional) | HTTP get (careful async) |
| `Data.OSC In/Out` | Early protocol win |

#### Definition of Done

- [ ] Script node creates nodes or sets params on pulse  
- [ ] Table drives Replicator later (or simple select)  

---

### Phase 9 — Geometry + Materials + Render (3D path)  
**Duration:** 8–14 weeks  

#### Minimum GeomOp set

| Node | Behavior |
|------|----------|
| `Geom.Grid` | Grid mesh |
| `Geom.Sphere` | Sphere |
| `Geom.Transform` | TRS |
| `Geom.Merge` | Merge meshes |
| `Geom.NoiseDeform` | Displace |
| `Geom.FromChan` optional | Instance data later |

#### Mat + Comp

| Node | Behavior |
|------|----------|
| `Mat.Constant` | Unlit color |
| `Mat.Phong` or `Mat.PBRBasic` | Basic lighting |
| `Mat.GLSL` | Custom shader material |
| `Comp.Geometry` | Holds mesh+mat |
| `Comp.Camera` | Camera |
| `Comp.Light` | Light |
| `Tex.Render` | Render scene to texture |

#### Definition of Done

- [ ] Render a lit mesh to texture → composite in Tex graph → window  
- [ ] Custom GLSL material hot reloads  

---

### Phase 10 — Media I/O (Video, Capture, Share)  
**Duration:** 6–10 weeks  

#### Tasks

10.1 `Tex.MovieFileIn` via FFmpeg (decode → GPU upload).  
10.2 `Tex.VideoDeviceIn` (Media Foundation / Decklink later).  
10.3 `Tex.NDI In/Out` (if SDK licensed).  
10.4 `Tex.Spout In/Out` (Windows).  
10.5 Audio device meter → Chan.  
10.6 Latency & drop-frame policy docs.  
10.7 License compliance file for FFmpeg/NDI.  

#### Definition of Done

- [ ] Play a video file through effects to window in real time  
- [ ] Spout out received by another app on same machine  

---

### Phase 10b — Projection Mapping & Multi-Output ⭐ NEO REALMS PRIORITY  
**Duration:** 6–10 weeks  
**Goal:** Warp, blend, and output to multiple projectors simultaneously — the core workflow for building façade activations, floor plans, and holograms.

> ⚠️ **This phase is pulled forward from the original Phase 15 backlog because projection mapping is Neo Realms' primary commercial workflow. It must be available before Year 1 is declared complete.**

#### Tasks

10b.1 **Multi-output window system:** Open N independent OS windows, each targeting a different display/projector output.  
10b.2 **`Tex.ProjectorOut` node:** Assigns a texture to a specific output window + screen index.  
10b.3 **Warp mesh editor (2D):** Load or interactively edit a per-output UV warp mesh (bezier patch or grid). Serialize to `.nfc` component.  
10b.4 **Softedge / edge-blending:** Overlap region detection, gradient blend mask computation, gamma-correct blending (industry standard linear blend curve).  
10b.5 **Output resolution overrides:** Each output can have independent resolution (e.g. one 4K, two 1080p).  
10b.6 **Black-level compensation:** Projector stacking lift correction.  
10b.7 **Test geometry overlay:** Grid / circle / crosshair pattern generator for alignment on-site.  
10b.8 **`Tex.NDI Out` integration:** Send warped output over NDI to media server if needed.  
10b.9 **Save/load warp + blend configs:** Stored per-show in `.nfp` project.  
10b.10 **On-site calibration UI:** Minimal fullscreen mode with warp grid overlay — operator adjusts points with mouse/keyboard without opening full editor.  

#### Definition of Done

- [ ] Two projectors show the same source with different warp meshes and a soft-blended overlap  
- [ ] Warp config survives save → load → show restart  
- [ ] On-site calibration UI usable by a non-engineer  
- [ ] 1080p per output at stable 60 FPS on RTX-class GPU  

#### Agent note

This is **Neo Realms' flagship capability.** Cook correctness and output stability under 8+ hour soak are higher priority than feature breadth here.

---

### Phase 11 — Protocols & Show Control  
**Duration:** 4–8 weeks  

| Protocol | Node examples |
|----------|----------------|
| MIDI | `Chan.MIDIIn` |
| OSC | `Data.OSCIn`, `Chan.OSCIn` |
| Serial | `Data.Serial` |
| DMX/Art-Net | later subphase |
| Keyboard/Mouse | `Chan.Mouse`, `Chan.Keys` |

#### Definition of Done

- [ ] MIDI CC drives a param live  
- [ ] OSC from phone/app updates a constant  

---

### Phase 12 — Performance, Stability, Profiling  
**Duration:** continuous; hard gate before production install  

#### Tasks

12.1 Cook profiler UI (per-node ms).  
12.2 GPU pass timing.  
12.3 Texture pool stats / leak detector.  
12.4 Long-soak test 8–24h.  
12.5 Deterministic fault injection (device lost).  
12.6 Crash reporter (internal).  
12.7 Frame pacing / vsync options.  

#### Definition of Done

- [ ] Mid-complexity company sample runs 4h+ without leak growth  
- [ ] Perf HUD shows node costs  

---

### Phase 13 — Plugin SDK  
**Duration:** 4–6 weeks  

#### Tasks

13.1 Stable C ABI or C++ plugin interface versioned.  
13.2 Sample plugin: `Tex.ExamplePlugin`.  
13.3 Discovery path for DLLs.  
13.4 SDK headers + example CMake.  
13.5 Document lifecycle: load, cook, unload.  

#### Definition of Done

- [ ] External DLL operator loads hot (or on startup) and cooks  

---

### Phase 14 — Company Workflow Pack + Training  
**Duration:** 2–6 weeks  

#### Tasks

14.1 Build the **actual company flagship graph** as a sample.  
14.2 Operator cheat sheets.  
14.3 Internal workshop doc.  
14.4 Template projects.  
14.5 Backup/IT install guide.  

#### Definition of Done

- [ ] Non-author engineer completes flagship task using only internal docs  

---

### Phase 15 — Expansion Toward “Almost Everything” (Ongoing Years)

Only after Phase 14. Treat as **backlog epics**, not free-for-all:

- Particles (GPU)  
- Instancing advanced  
- UI panel Comp system (build control panels inside graphs)  
- Replicator / clone patterns  
- Physics lite  
- VR/AR outputs  
- Database nodes  
- WebRTC  
- Advanced PBR / deferred  
- Timeline / keyframe editor  
- macOS port  
- Binary project format  
- Collaboration / remote director  

**Rule:** Each epic gets its own mini-roadmap + operator specs before coding.

---

## 6. Operator Specification Standard (Anti-Hallucination)

Before implementing any operator, create:

`docs/operator-spec/Tex.Blur.md` (example)

Required sections:

1. Name / family / version  
2. Pins (in/out types)  
3. Params (types, defaults, ranges)  
4. Cook semantics (when dirty, time dependent?)  
5. GPU/CPU resources  
6. Error cases  
7. Test plan  
8. Phase ownership  

**Agents may not implement operators without a spec file.**

---

## 7. Testing Strategy

| Level | What |
|-------|------|
| Unit | Topo sort, params, math ChanOps |
| Cook integration | Small graphs JSON fixtures |
| GPU smoke | Create device, cook Tex.Constant |
| Golden (optional) | Image compare with tolerance |
| Perf | Frame time budgets on sample graphs |
| Soak | Long run memory |

**CI gate:** unit + cook tests must pass on main.

---

## 8. Performance Budgets (Initial)

| Scenario | Target |
|----------|--------|
| UI only idle | < 5 ms CPU average |
| Simple Tex graph 1080p | 60 FPS stable |
| Movie + 5 effects 1080p | ≥ 30 FPS min, 60 goal |
| 200-node graph UI interact | pan/zoom responsive |

Revise budgets after Phase 12 measurements.

---

## 9. Security & IT (Private Company Tool)

- Scripts are trusted internal code — still log & catch.  
- No silent network exfil nodes without review.  
- Plugin DLL loading only from approved directories.  
- Media paths sandboxed to project root option.  
- Secrets never in `.nfp` (use env for API keys if Web nodes exist).  

---

## 10. Team Shape (Recommended)

| Role | Count (MVP) | Notes |
|------|-------------|-------|
| Graphics systems (C++/Vulkan) | 1–2 | Critical |
| Engine/graph | 1 | Cook model owner |
| Tools UI | 1 | ImGui node editor |
| Tech artist / op author | 1 | Shaders + specs |
| Python/scripting | 0.5 | Can share engine |
| QA | 0.5–1 | Soak + hardware |

Solo is possible for a thin MVP but will stall before Phase 9–10 quality.

---

## 11. Milestone Timeline (Indicative)

| Milestone | Phases | Calendar (full-time small team) |
|-----------|--------|----------------------------------|
| M0 Shell | 0–1 | Month 1–2 |
| M1 Cook core | 2–3 | Month 2–4 |
| M2 Live Tex | 4–5 | Month 4–7 |
| M3 Project + Chan | 6–7 | Month 7–9 |
| M4 Script + IO start | 8,10 partial | Month 9–11 |
| M5 3D path | 9 | Month 11–14 |
| M6 Show-ready internal | 11–14 | Month 14–18 |

Parallelism: UI (5) can start late Phase 4; do not start Phase 9 before 4–7 stable.

---

## 12. STATUS.md Template

```markdown
# NodeForge Status

**Active phase:** 0
**Phase name:** Project Genesis
**Updated:** YYYY-MM-DD
**Branch policy:** main protected; work on phase/N-*

## In progress
- [ ] task

## Blocked
- none

## Last DoD sign-off
- none
```

---

## 13. AGENTS.md Template (Short)

```markdown
# Agent Rules — NodeForge

1. Read docs/02-BUILD-ROADMAP-A-to-Z.md
2. Only implement the active phase in STATUS.md
3. No TouchDesigner asset/UI copying; clean-room only
4. New operators require docs/operator-spec/*.md first
5. Architecture changes need docs/adr/ entry
6. Run tests before claiming done
7. Prefer small PRs per task
```

---

## 14. First 30 Concrete Implementation Tasks (Kickoff List)

Use this as the initial ticket breakdown after docs exist:

1. Create CMake+vcpkg project `nodeforge`  
2. GLFW window bootstrap  
3. Logging with spdlog  
4. Vulkan instance + device  
5. Swapchain  
6. VMA  
7. Triangle clear  
8. Texture upload path  
9. Display texture  
10. `core` ID types  
11. `Graph` data structures  
12. Registry  
13. Dirty flags  
14. Topo sort  
15. Cook context  
16. `Chan.Const` + `Chan.MathAdd`  
17. Cook tests  
18. JSON graph serialize  
19. Param model  
20. ImGui integration  
21. Node editor canvas MVP  
22. Create Tex.Constant  
23. Tex cook path  
24. Wire Tex to present  
25. Param panel  
26. Python embed hello  
27. pybind set param  
28. Save `.nfp`  
29. Load `.nfp`  
30. Sample: Noise→Blur→Window  

---

## 15. Technology Alternatives (Allowed Substitutions)

Agents may substitute **only** with ADR:

| Default | Allowed alternative | When |
|---------|---------------------|------|
| GLFW | SDL3 | Need audio/gamepad earlier |
| ImGui node editor | custom or imnodes | Licensing/maintenance |
| Vulkan | DX12 | Windows-only team expertise |
| pybind11 | nanobind | Preference |
| nlohmann/json | simdjson | Perf on huge projects |
| FFmpeg | platform decoders only | License simplification |

**Not allowed without executive decision:** rewriting core in C#, Java, or pure Python.

---

## 16. Mapping “Everything TD Has” → NodeForge Phases

| Capability area | Phase |
|-----------------|-------|
| Node graph + cook | 2 |
| Parameters + Python | 3 |
| Texture/video effects | 4, 10 |
| Visual editor | 5 |
| Project/components | 6 |
| Channels/control | 7 |
| Tables/scripts/OSC | 8, 11 |
| 3D + materials + render | 9 |
| Devices/share | 10 |
| **Projection mapping / multi-output / softedge** ⭐ | **10b** |
| Protocols/show control | 11 |
| Plugins | 13 |
| Particles/UI comps/VR/advanced hologram | 15+ |

This is how you get to “everything” **without lying to yourselves about year one.**

---

## 17. Quality Bar Before Internal Production Use

Must all be true:

1. Flagship company workflow graph exists  
2. Save/load reliable  
3. 4h soak passed  
4. Cook errors are visible, not silent  
5. Perf HUD available  
6. Two engineers can operate without the original author  
7. License file lists third-party notices  
8. Backup/IT can install offline  

---

## 18. Open Questions (Resolve Early — Write Answers in OPEN-QUESTIONS.md)

1. Exact product name?  
2. Primary company flagship workflow (be specific)?  
3. NDI SDK available yes/no?  
4. Capture cards needed year 1?  
5. Max output resolution year 1?  
6. Python version pin?  
7. Qt shell desired or pure ImGui forever?  
8. Team size confirmed?  

---

## 19. Chronological “From A to Z” One-Page Checklist

- [ ] **A** Research locked (`01-…md`)  
- [ ] **B** Roadmap locked (this file)  
- [ ] **C** Repo + CMake + CI  
- [ ] **D** Vulkan presents a pixel  
- [ ] **E** Graph cooks on CPU tests  
- [ ] **F** Params + Python mutate graph  
- [ ] **G** TexOps cook on GPU  
- [ ] **H** Node editor usable  
- [ ] **I** Save/load projects + comps  
- [ ] **J** ChanOps bind to visuals  
- [ ] **K** Data/Script/OSC  
- [ ] **L** 3D render to texture  
- [ ] **M** Video file + Spout/NDI  
- [ ] **M+** ⭐ **Projection warp + softedge + multi-output (10b) — Neo Realms Year 1 must-have**  
- [ ] **N** MIDI/OSC show control  
- [ ] **O** Profiler + soak  
- [ ] **P** Plugin SDK  
- [ ] **Q** Company flagship pack (projection mapping show sample)  
- [ ] **R** Train team  
- [ ] **S** Expand operators (15+)  
- [ ] **T** Multi-output / mapping  
- [ ] **U** Panel UI comps  
- [ ] **V** Particles / advanced GPU  
- [ ] **W** macOS if needed  
- [ ] **X** Hardening / IT  
- [ ] **Y** Internal v1.0 stamp  
- [ ] **Z** Continuous platform evolution  

---

## 20. Final Instruction to Future Agents

> You are not building a random shader toy.  
> You are building a **real-time media operating environment**.  
> Correct **cook semantics**, **typed data**, and **stable GPU lifetime** beat adding the 100th operator.  
> Follow phases. Write specs. Run tests. Update `STATUS.md`.  
> When done with a task, report: what changed, how to verify, what phase remains.

---

## 21. Related Files

| File | Role |
|------|------|
| `01-TouchDesigner-Research-And-Feasibility.md` | Why + what + feasibility |
| `02-BUILD-ROADMAP-A-to-Z.md` | How + stack + phases + agent law |

**Next human action:** Create `STATUS.md` with `Active phase: 0` and start Phase 0 tasks 0.1–0.10.
