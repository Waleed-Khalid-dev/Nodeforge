# NodeForge

> Real-time node-based media engine for projection mapping, interactive holograms, and walkable floor-plan visualizations.

**Company:** Neo Realms — Pakistan's first projection mapping & interactive hologram company  
**Owner:** Waleed Khalid (solo, part-time)  
**Status:** 🔵 Phase 0 — Project Genesis  
**Git:** https://github.com/Waleed-Khalid-dev/Nodeforge (private)

---

## What is NodeForge?

A private Windows-first real-time media engine in the same category as TouchDesigner:
node graph, GPU textures, channels, 3D geometry, Python scripting, and **multi-projector output**.

Built to power Neo Realms' commercial shows:
- 🏢 **Façade-scale projection mapping** on buildings (with warp + softedge)
- 👋 **Interactive gesture layer** (touchless wave/swipe surfaces)
- 🏠 **Walkable 1:1 floor-plan projections** for real-estate developers
- 💎 **Holograms** for product reveals and events

---

## Tech Stack

| Layer | Choice |
|-------|--------|
| Language | C++23 |
| GPU | Vulkan 1.3 + VMA + vk-bootstrap |
| UI | Dear ImGui (docking) + imgui-node-editor |
| Build | CMake 3.28 + Ninja + vcpkg |
| Scripting | Python 3.11 (embedded) + pybind11 |
| Video | FFmpeg + NDI SDK v6.3.2 + Spout2 |
| Gesture | Intel RealSense SDK + MediaPipe |
| Projection | OpenCV homography + custom warp GLSL shader |

---

## Roadmap

See [`docs/02-BUILD-ROADMAP-A-to-Z.md`](docs/02-BUILD-ROADMAP-A-to-Z.md) — 15 phases from empty shell to full production tool.

**Phase 10b** (Projection Mapping & Multi-Output) is the Year-1 must-have for Neo Realms.

---

## Building

### Prerequisites

- Windows 10/11 x64
- Visual Studio 2022 (MSVC v143)
- CMake 3.28+
- Ninja
- vcpkg (set `VCPKG_ROOT` env var)

### Build

```powershell
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
cmake --build build
.\build\bin\nodeforge.exe
```

---

## Current Phase: 0 — Project Genesis

- [x] Folder structure
- [x] CMakeLists.txt + vcpkg.json
- [x] GLFW window opens and closes
- [x] .gitignore, .editorconfig, .clang-format
- [x] Planning docs in docs/
- [ ] ADR-0001 (stack confirmation)
- [ ] ADR-0002 (file formats)
- [ ] ADR-0003 (naming conventions)
- [ ] CI: GitHub Actions Windows build

---

## License

Private — Neo Realms internal use only. Not for distribution.  
Clean-room implementation. No TouchDesigner assets or binaries used.
