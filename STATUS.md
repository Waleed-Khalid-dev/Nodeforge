# NodeForge Status

**Active phase:** 10  
**Phase name:** Media I/O (Video, Capture, Share) / Phase 10b (Projection Mapping & Multi-Output)  

**Updated:** 2026-08-20  
**Branch policy:** main protected; work on `phase/0-*` branches when git is initialized  
**Git remote:** https://github.com/Waleed-Khalid-dev/Nodeforge (make private before creating)

## In progress

- [ ] Phase 10 kickoff (Media I/O: FFmpeg MovieFileIn, VideoDeviceIn, NDI In/Out, Spout In/Out / Phase 10b Projection Mapping & Multi-Output)

## Completed

- [x] Phase 0: Repository, clean-room setup, roadmap, baseline CMake & dependencies
- [x] Phase 1: GPU Foundation (Vulkan 1.3, Dynamic Rendering, VMA, Swapchain, Texture2D upload & present, Headless tests)
- [x] Phase 2: Graph Runtime (Node, Pin, Wire, Graph, NodeRegistry, Topo sort, Cycle rejection, Hybrid dirty propagation, 4 starter operators, JSON roundtrip, 10/10 tests)
- [x] Phase 3: Parameter System + Expressions + Python Embed (Dual-mode Param/Expr, pybind11 nodeforge module, GIL policy, exception recovery, 16/16 tests passing)
- [x] Phase 4: TexOp Pipeline (GPU Nodes) (Base TexOp, lease-based TexturePool, runtime GLSL ShaderCompiler with embedded SPIR-V, FullscreenPass dynamic rasterizer, ComputePass compute dispatcher, 10 core TexOps: Null, Constant, Noise, LoadImage, Transform, Composite, Blur, Level, Resolution, ToWindow, 29/29 tests passing, 0 leaks across 10,000 frames)
- [x] Phase 5: Editor UI (Node Graph IDE) (Custom ImDrawList infinite canvas with 0.2x-2.5x pan/zoom & bezier wires, multi-node selection & move, TAB OP Create palette with fuzzy search & family filtering, multi-page Parameter Inspector with dynamic C/E toggle, live GPU Texture & Channel Viewer, Log Console & Python REPL prompt, Timeline transport bar with frame scrubber, Main Menu Bar, full Command Pattern Undo/Redo history, 35/35 unit and benchmark tests passing)
- [x] Phase 6: Project System & Components (Complete `.nfp` JSON v1 schema roundtrip, relative path resolution, ContainerComp nested subnetwork execution, InOp/OutOp boundary pin sync, `.nfc` Component import/export, 60s background autosave & crash recovery detection, interactive breadcrumb navigation bar, 41/41 unit & benchmark tests passing)
- [x] Phase 7: ChanOps (Control / Animation / Audio-lite) (SIMD-accelerated ChannelBuffer, 10 core ChanOps: Constant, Time, LFO, Noise, Math, Filter, Merge, Select, Trail, AudioFileInSynthesizer, dual-direction ChanToTex & TexToChan interop, high-resolution vector scope oscilloscope in ViewerPanel, 57/57 tests passing)
- [x] Phase 8: DataOps + Script Nodes (DAT) (Unified 2D DataTable, RFC 4180 CSV/TSV parser/serializer, raw text, JSON Pointer parsing, dual-trigger ScriptDataOp Python hooks, non-blocking UDP OSC In/Out receiver/sender, async WebDataOp HTTP client, ChanToData & DataToChan interop bridges, interactive ImGui spreadsheet table with virtual scrolling clipper, 69/69 tests passing, >100k cells/sec benchmark, 0 leaks across 10,000 frames)
- [x] Phase 9: Geometry + Materials + Render (3D Path / GeomOps / SOP) (Comprehensive GeometryData interleaved vertex mesh data model, 5 procedural primitives: Grid, Sphere, Box, Torus, Cylinder, 4 modifiers: Transform, Merge, NoiseDeform, Normals, 1 interop: ChanToGeom, 3 materials: Constant, Phong, GLSL, 3 scene comps: Camera, Light, Geometry with dual-source GPU instancing, RenderTexOp with Vulkan 1.3 dynamic rasterizer, interactive 3D Arcball / Turntable orbit viewport with floor grid in ViewerPanel, mini isometric 3D wireframe preview on canvas nodes, 80/80 automated tests passing, 0 GPU memory leaks over 10,000 frames)

## Blocked

- none

## Docs present

- [x] `01-TouchDesigner-Research-And-Feasibility.md`
- [x] `02-BUILD-ROADMAP-A-to-Z.md` — Phase 10b added (Projection Mapping ⭐), Section 2.7 library list added
- [x] `STATUS.md` (this file)
- [x] `AGENTS.md`
- [x] `docs/OPEN-QUESTIONS.md` — all 9 questions answered 2026-08-09
- [x] `docs/adr/ADR-0001-stack.md`
- [x] `docs/adr/ADR-0002-file-formats.md`
- [x] `docs/adr/ADR-0003-naming.md`
- [x] `docs/adr/ADR-0004-graph-runtime.md`
- [x] `docs/adr/ADR-0006-gpu-texop-pipeline.md`
- [x] `docs/adr/ADR-0007-chanop-pipeline-and-audio.md`
- [x] `docs/adr/ADR-0008-dataop-pipeline-and-scripting.md`
- [x] `docs/adr/ADR-0009-geometry-materials-and-render-pipeline.md`
- [x] `docs/operator-spec/GridGeomOp.md`
- [x] `docs/operator-spec/SphereGeomOp.md`
- [x] `docs/operator-spec/BoxGeomOp.md`
- [x] `docs/operator-spec/TorusGeomOp.md`
- [x] `docs/operator-spec/CylinderGeomOp.md`
- [x] `docs/operator-spec/TransformGeomOp.md`
- [x] `docs/operator-spec/MergeGeomOp.md`
- [x] `docs/operator-spec/NoiseDeformGeomOp.md`
- [x] `docs/operator-spec/NormalsGeomOp.md`
- [x] `docs/operator-spec/ChanToGeomOp.md`
- [x] `docs/operator-spec/ConstantMatOp.md`
- [x] `docs/operator-spec/PhongMatOp.md`
- [x] `docs/operator-spec/GLSLMatOp.md`
- [x] `docs/operator-spec/CameraComp.md`
- [x] `docs/operator-spec/LightComp.md`
- [x] `docs/operator-spec/GeometryComp.md`
- [x] `docs/operator-spec/RenderTexOp.md`

## Last DoD sign-off

- Phase 0 complete (2026-08-11)
- Phase 1 complete (2026-08-18)
- Phase 2 complete (2026-08-19)
- Phase 3 complete (2026-08-19)
- Phase 4 complete (2026-08-19)
- Phase 5 complete (2026-08-19)
- Phase 6 complete (2026-08-19)
- Phase 7 complete (2026-08-19)
- Phase 8 complete (2026-08-19)
- Phase 9 complete (2026-08-20)

## Notes

- Codename: **NodeForge** (rename allowed; do not use TouchDesigner trademarks in product name).
- Private company use. Clean-room implementation only.
- Follow `02-BUILD-ROADMAP-A-to-Z.md` strictly.
- **Owner:** Waleed Khalid (solo, part-time).
- **Company:** Neo Realms — projection mapping, interactive holograms, walkable floor plans.
- **Flagship workflow:** Multi-projector façade mapping + gesture interaction (Phase 10b is Year-1 must-have).
- **Python embed version:** 3.11 (system 3.13 untouched — they coexist).
- **Hardware target:** RTX 3060 12 GB minimum; max canvas 5760×1080.
- **Third-party libs approved:** See Section 2.7 of roadmap. Use vcpkg where available.
