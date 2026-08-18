---
type: project
created: 2026-08-18
updated: 2026-08-19
---

# Project Conventions — NodeForge

## Environment & Path Rules
- **Bracketed Directory**: Workspace is `D:\[Project]\Touch Designer`. Always use `Set-Location -LiteralPath` in PowerShell and `git -C "D:\`[Project`]\Touch Designer"` to avoid bracket wildcard expansion.
- **CMake Patching**: `patch_cmake.py` patches vcpkg `file(GLOB)` loops into literal `if(EXISTS)` rules to handle the bracketed directory without breaking CMake 4.x.

## Code & Naming Conventions
- **Namespace**: `namespace nf { ... }` for engine core, `namespace gpu { ... }` for low-level Vulkan utilities.
- **Prefixes**: `NF_` for macros and preprocessor definitions.
- **Operator Families**: `TexOp` (TOP), `ChanOp` (CHOP), `GeomOp` (SOP), `MatOp` (MAT), `DataOp` (DAT), `Comp` (COMP).
- **Clean Code**: Concise, direct, self-documenting, no over-engineering. Clean-room implementation only.

## Development & Git Workflow
- **Roadmap Alignment**: Work strictly within the active phase defined in `STATUS.md` and `02-BUILD-ROADMAP-A-to-Z.md`.
- **Operator Specs & ADRs**: Every new operator requires `docs/operator-spec/<Name>.md`, and major architecture decisions require `docs/adr/ADR-XXXX-title.md` before coding.
- **Atomic Commits**: Make clean, structured commits (feat, fix, docs, test, checkpoint) and push regularly to `https://github.com/Waleed-Khalid-dev/Nodeforge.git`.
