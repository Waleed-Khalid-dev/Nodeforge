---
type: project
created: 2026-05-25
updated: 2026-05-25
---

# Project Conventions

## Git & Shell Workflow
- Always create a new dedicated branch for major code changes.
- Branch name format should follow: `feature/[task-slug]` or `fix/[bug-slug]`.
- Project path has brackets: `D:\[Project]\Touch Designer`. Always use `Set-Location -LiteralPath` in PowerShell or `git -C "<path>"` for git commands.
- Run `python patch_cmake.py` whenever vcpkg dependencies are installed or updated.
