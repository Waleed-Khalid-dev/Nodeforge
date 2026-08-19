---
type: project
created: 2026-08-18
updated: 2026-08-20
---

# Technical Decisions & Environment Constraints

## 1. Network / VPN constraints
Because of SSL inspection and certificate issues in the user's environment, `vcpkg` fails to download source archives (`curl 35: schannel failed`). The user must have a VPN (1.1.1.1 or Cloudflare WARP) active during the initial setup or anytime `vcpkg install` fetches new packages.

## 2. CMake 4.x vcpkg bug (minizip)
CMake 4.x is extremely strict about `IMPORTED_IMPLIB` on Ninja. `vcpkg`'s built-in `zlib` generates a `ZLIB-shared-debug.cmake` without a fallback `IMPORTED_IMPLIB`, which causes generation to fail for `minizip` and others.
*Solution:* We patched `C:\dev\vcpkg\ports\zlib\portfile.cmake` to append the fallback property to `ZLIB-shared.cmake` before installation. If the environment is ever rebuilt on a new machine, this patch may need to be reapplied or `minizip` will fail on CMake 4.x.

## 3. third_party structure
SDKs that cannot be easily fetched via vcpkg (DeckLink, Spout2, Python Embed) are stored directly in `third_party/`.

## 4. Bracketed Project Path Bug (`[Project]`)
The workspace directory path contains brackets: `D:\[Project]\Touch Designer`.
- **Root cause:** CMake's `file(GLOB ...)` interprets brackets `[...]` as regular expression character classes. As a result, standard vcpkg target export files (e.g. `ZLIB-shared.cmake`, `pugixml-targets.cmake`, `minizip-shared.cmake`) that use `file(GLOB _cmake_config_files "${CMAKE_CURRENT_LIST_DIR}/...-*.cmake")` fail to find any debug/release configuration files, leaving `IMPORTED_CONFIGURATIONS` and `IMPORTED_IMPLIB` empty.
- **Solution (Option B):** `patch_cmake.py` scans `vcpkg_installed` and replaces `file(GLOB)` loops with literal `if(EXISTS ...)` and `include(...)` calls, and injects missing `IMPORTED_IMPLIB_<CONFIG>` properties for CMake 4.x. This script runs automatically during setup/build.
- **PowerShell / Git rule:** In PowerShell, always use `Set-Location -LiteralPath` or `git -C "D:\[Project]\Touch Designer"` to avoid PowerShell wildcard expansion on `[Project]`.

## 5. Phase 8 DataOp & Scripting Architecture (ADR-0008)
- **Unified 2D DataTable:** Stores cells in dynamic string vectors with typed conversion helpers, named column/row headers, and RFC 4180 CSV/TSV parser/serializer.
- **Dual Cooking & Event Hooks:** `ScriptDataOp` executes Python scripts on DAG cook (`onCook(dat)`) or parameter pulse (`onPulse(dat, param)`), with full GIL safety and try-catch error boundary.
- **Non-blocking UDP & HTTP Network I/O:** `OSCInOp` listens on dedicated background UDP thread into a thread-safe ring buffer; `WebDataOp` uses async worker thread to ensure the rendering loop never drops frames on network latency.
- **Bi-directional Bridges:** `ChanToDataOp` (ChopToDat) and `DataToChanOp` (DatToChop) allow seamless conversion between numerical waveforms and tabular spreadsheets.
