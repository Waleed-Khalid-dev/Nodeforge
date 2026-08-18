# NodeForge Status

**Active phase:** 3  
**Phase name:** Parameter System + Expressions + Python Embed  

**Updated:** 2026-08-19  
**Branch policy:** main protected; work on `phase/0-*` branches when git is initialized  
**Git remote:** https://github.com/Waleed-Khalid-dev/Nodeforge (make private before creating)

## In progress

- [ ] Phase 3 kickoff (Parameter System, Expressions, Python 3.11 embed, pybind11 integration)

## Completed

- [x] Phase 0: Repository, clean-room setup, roadmap, baseline CMake & dependencies
- [x] Phase 1: GPU Foundation (Vulkan 1.3, Dynamic Rendering, VMA, Swapchain, Texture2D upload & present, Headless tests)
- [x] Phase 2: Graph Runtime (Node, Pin, Wire, Graph, NodeRegistry, Topo sort, Cycle rejection, Hybrid dirty propagation, 4 starter operators, JSON roundtrip, 10/10 tests)

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
- [x] `docs/operator-spec/ConstantChanOp.md`
- [x] `docs/operator-spec/MathChanOp.md`
- [x] `docs/operator-spec/ConstantTexOp.md`
- [x] `docs/operator-spec/TransformTexOp.md`
- [x] `docs/api/gpu-api.md`

## Last DoD sign-off

- Phase 0 complete (2026-08-11)
- Phase 1 complete (2026-08-18)
- Phase 2 complete (2026-08-19)

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
