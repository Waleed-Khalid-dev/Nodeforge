---
type: project
created: 2026-08-18
updated: 2026-08-19
---

# NodeForge Architecture

## Overview
NodeForge is a real-time node-based visual development platform for generative graphics, projection mapping, interactive holograms, and walkable floor plans (Neo Realms flagship workflow).

## Tech Stack
- **Language & Standard**: C++23 (MSVC `/W4 /WX /utf-8 /std:c++latest`)
- **Graphics API**: Vulkan 1.3 with dynamic rendering, VMA (VulkanMemoryAllocator), and `shaderc` runtime GLSL compilation
- **Windowing & UI**: GLFW3 + Dear ImGui
- **Graph Runtime**: Hybrid pull-on-demand with push dirty invalidation, type-safe `PinValue` (`std::variant`), Kahn's topological sorting, reachability-based cycle rejection, and JSON graph serialization
- **Scripting & Expressions**: Embedded CPython 3 via `pybind11` (Phase 3)
- **Logging & Utilities**: `spdlog`, `fmt`, `nlohmann_json`, `stb`
- **Testing**: GoogleTest (unit tests) + Google Benchmark

## Key Subsystems & Status
- **GPU Subsystem (`src/gpu/`)**: `Device`, `Swapchain`, `Texture2D`, `FrameResources`, `TexturePool`, `ShaderCompiler`, `FullscreenPass`, `ComputePass`. Supports headless GPU initialization and 60 FPS presentation. (Phase 1 & Phase 4 ✅)
- **Graph Runtime (`src/graph/`)**: `Node`, `Pin`, `Wire`, `Graph`, `NodeRegistry`, `GraphSerializer`. (Phase 2 ✅)
- **Parameter & Python Subsystem (`src/param/`, `src/python/`)**: Dual-mode parameters (`Constant` vs `Expression`), `PythonEngine` singleton with GIL guard, `nodeforge` module bindings. (Phase 3 ✅)
- **TexOp Pipeline (`src/operators/tex/`)**: 10 Core GPU Texture Operators (`Null`, `Constant`, `Noise`, `LoadImage`, `Transform`, `Composite`, `Blur`, `Level`, `Resolution`, `ToWindow`), Texture pool with 0 memory leaks across 10,000 frames. (Phase 4 ✅)
- **Studio Editor UI (`src/ui/`)**: ImDrawList infinite canvas (0.2x-2.5x pan/zoom, bezier wires, multi-select), OP palette, Parameter inspector, Viewer, Console/REPL, Timeline transport, UndoManager. (Phase 5 ✅)
- **Project System & Components (`src/project/`, `src/operators/comp/`)**: ProjectSerializer `.nfp` JSON v1, ComponentSerializer `.nfc`, ContainerComp subnetwork hierarchy, InOp/OutOp boundary pin sync, 60s Autosave & Crash recovery, Breadcrumb navigation. (Phase 6 ✅)
- **ChanOp Pipeline (`src/operators/chan/`)**: (Phase 7 🔨 Active next)
