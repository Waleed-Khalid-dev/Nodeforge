# Agent Rules — NodeForge

1. **Read first:** `01-TouchDesigner-Research-And-Feasibility.md` and `02-BUILD-ROADMAP-A-to-Z.md`.  
2. **Only implement the active phase** in `STATUS.md`. Do not skip phases.  
3. **Clean-room only:** no TouchDesigner decompilation, no copying TD UI/assets/icons.  
4. **New operators require** `docs/operator-spec/<Name>.md` before code.  
5. **Architecture changes require** `docs/adr/ADR-XXXX-title.md`.  
6. **Prefer cook correctness + tests** over adding many operators.  
7. **Small tasks / small PRs.** Report: changes, how to verify, remaining phase work.  
8. If unclear: add a note to `docs/OPEN-QUESTIONS.md` — do not invent APIs that contradict the roadmap.  
9. Naming: `nf::`, `NF_`, product **NodeForge**; families TexOp / ChanOp / GeomOp / MatOp / DataOp / Comp.  
10. Windows-first. C++20/23 + Vulkan + ImGui + Python embed as specified in the roadmap.  
11. **Do not reinvent approved libraries.** Before writing any utility code, check **Section 2.7** of `02-BUILD-ROADMAP-A-to-Z.md`. If a library already covers it (e.g. vk-bootstrap for Vulkan init, imgui-node-editor for the canvas, pybind11 for Python) — use it, do not rewrite it.  
12. **Project owner:** Waleed Khalid (solo, part-time). **Git remote:** https://github.com/Waleed-Khalid-dev/Nodeforge (private). **Company:** Neo Realms. All work serves the projection mapping + gesture + floor-plan flagship workflow.  
