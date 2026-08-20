# Memory Index

## User Preferences
- [user] Professional, direct, and concise communication style with structured implementation plans -> user-preferences.md
- [user] Requires VPN (WARP/1.1.1.1) for large vcpkg package downloads -> tech-decisions.md

## Project & Architecture
- [project] NodeForge Core Stack: C++23, Vulkan 1.3, Dear ImGui, CPython 3 embed, vcpkg -> architecture.md
- [project] Workspace directory has brackets `D:\[Project]\Touch Designer`; use `-LiteralPath` in PowerShell and `git -C` with backtick escapes -> project-conventions.md
- [project] Phase 0 to Phase 13 (GPU presentation, DAG Runtime, Python 3 Embed, TexOp Pipeline, Studio Editor UI, Project System & Components, ChanOp SIMD Audio/Channels, DataOps & Scripting, Geometry Engine & 3D Vulkan Render Pipeline, Media I/O & Projection Mapping Suite, Protocols & Show Control, Performance Profiler & Crash Diagnostics, Plugin SDK, Kiosk Player, Packaging & CI/CD, 105/105 tests) COMPLETE -> STATUS.md
- [project] Active Phase: Phase 14 (Company Workflow Pack + Training) -> STATUS.md
- [project] Hybrid pull-on-demand with push dirty invalidation, PinValue variant, and Kahn's topo-sort -> docs/adr/ADR-0004-graph-runtime.md
- [project] Parameter System & Python Embedding Architecture (dual-mode Constant/Expression, pybind11, GIL management) -> docs/adr/ADR-0005-parameter-system-and-python.md
- [project] GPU TexOp Pipeline & Shader Architecture (FullscreenPass, ComputePass, dynamic raster/compute) -> docs/adr/ADR-0006-gpu-texop-pipeline.md
- [project] ChanOp Pipeline & Audio Architecture (SIMD contiguous ChannelBuffer, dual time-slicing, 3-tier parameter binding) -> docs/adr/ADR-0007-chanop-pipeline-and-audio.md
- [project] DataOp Pipeline & Scripting Architecture (DataTable 2D grid, Python onCook/onPulse, OSC In/Out, Web async HTTP) -> docs/adr/ADR-0008-dataop-pipeline-and-scripting.md
- [project] 3D Geometry Engine, Materials & Vulkan Dynamic Render Pipeline (GeometryData interleaved vertex mesh, 10 GeomOps, 3 MatOps, Camera/Light/Geometry Comps, Scene3DPass, RenderTexOp) -> docs/adr/ADR-0009-geometry-materials-and-render-pipeline.md
- [project] Media I/O, Texture Sharing & Projection Mapping Architecture (VideoDecoder, FrameRingBuffer, Spout2/NDI zero-copy interop, DisplayManager, WarpMesh 2D Bezier grid, WarpBlendPass) -> docs/adr/ADR-0010-media-io-and-projection-mapping.md
- [project] Protocols & Show Control Architecture (MidiManager, SerialPort Win32 Overlapped, ArtNetEngine UDP 6454 DMX512, InputManager multi-scope Mouse/Keyboard) -> docs/adr/ADR-0011-protocols-and-show-control.md
- [project] Performance Profiling, Stability & Crash Recovery Architecture (CookProfiler, GpuTimerPool VkQueryPool, TexturePool leak detector, ProfilerPanel, PerformanceHUD, Win32 SEH CrashReporter) -> docs/adr/ADR-0012-performance-profiling-and-stability.md
- [project] Plugin SDK, Dynamic Operator Runtime, Kiosk Player, Packaging & CI/CD Architecture (C ABI, C++23 SDK, PluginNodeProxy, PluginManager hot-reload, nodeforge_player.exe, CPack NSIS/ZIP) -> docs/adr/ADR-0013-plugin-sdk-and-packaging.md
