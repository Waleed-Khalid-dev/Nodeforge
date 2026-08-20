# Memory Index

## User Preferences
- [user] Professional, direct, and concise communication style with structured implementation plans -> user-preferences.md
- [user] Requires VPN (WARP/1.1.1.1) for large vcpkg package downloads -> tech-decisions.md

## Project & Architecture
- [project] NodeForge Core Stack: C++23, Vulkan 1.3, Dear ImGui, CPython 3 embed, vcpkg -> architecture.md
- [project] Workspace directory has brackets `D:\[Project]\Touch Designer`; use `-LiteralPath` in PowerShell and `git -C` with backtick escapes -> project-conventions.md
- [project] Phase 0 to Phase 9 (GPU presentation, DAG Runtime, Python 3 Embed, TexOp Pipeline, Studio Editor UI, Project System & Components, ChanOp SIMD Audio/Channels, DataOps & Scripting, Geometry Engine & 3D Vulkan Render Pipeline, 80/80 tests) COMPLETE -> STATUS.md
- [project] Active Phase: Phase 10 (Media I/O: Video, Capture, Share) / Phase 10b (Projection Mapping & Multi-Output ⭐) -> STATUS.md
- [project] Hybrid pull-on-demand with push dirty invalidation, PinValue variant, and Kahn's topo-sort -> docs/adr/ADR-0004-graph-runtime.md
- [project] Parameter System & Python Embedding Architecture (dual-mode Constant/Expression, pybind11, GIL management) -> docs/adr/ADR-0005-parameter-system-and-python.md
- [project] GPU TexOp Pipeline & Shader Architecture (FullscreenPass, ComputePass, dynamic raster/compute) -> docs/adr/ADR-0006-gpu-texop-pipeline.md
- [project] ChanOp Pipeline & Audio Architecture (SIMD contiguous ChannelBuffer, dual time-slicing, 3-tier parameter binding) -> docs/adr/ADR-0007-chanop-pipeline-and-audio.md
- [project] DataOp Pipeline & Scripting Architecture (DataTable 2D grid, Python onCook/onPulse, OSC In/Out, Web async HTTP) -> docs/adr/ADR-0008-dataop-pipeline-and-scripting.md
- [project] 3D Geometry Engine, Materials & Vulkan Dynamic Render Pipeline (GeometryData interleaved vertex mesh, 10 GeomOps, 3 MatOps, Camera/Light/Geometry Comps, Scene3DPass, RenderTexOp) -> docs/adr/ADR-0009-geometry-materials-and-render-pipeline.md
