# Memory Index

## User Preferences
- [user] Professional, direct, and concise communication style with structured implementation plans -> user-preferences.md
- [user] Requires VPN (WARP/1.1.1.1) for large vcpkg package downloads -> tech-decisions.md

## Project & Architecture
- [project] NodeForge Core Stack: C++23, Vulkan 1.3, Dear ImGui, CPython 3 embed, vcpkg -> architecture.md
- [project] Workspace directory has brackets `D:\[Project]\Touch Designer`; use `-LiteralPath` in PowerShell and `git -C` with backtick escapes -> project-conventions.md
- [project] Phase 0 (Clean-room repo setup), Phase 1 (GPU presentation pipeline & headless tests), Phase 2 (DAG Graph Runtime, 10/10 tests), and Phase 3 (Parameter System + Python 3 Embed + Expressions, 16/16 tests) COMPLETE -> STATUS.md
- [project] Active Phase: Phase 4 (TexOp Pipeline - GPU Nodes) -> STATUS.md
- [project] Hybrid pull-on-demand with push dirty invalidation, PinValue variant, and Kahn's topo-sort -> docs/adr/ADR-0004-graph-runtime.md
- [project] Parameter System & Python Embedding Architecture (dual-mode Constant/Expression, pybind11, GIL management) -> docs/adr/ADR-0005-parameter-system-and-python.md
