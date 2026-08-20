# ADR-0013: Plugin SDK, Dynamic Operator Runtime, Standalone Kiosk Player, and Distribution Architecture

## Status
Accepted

## Context
As NodeForge scales beyond core built-in operators toward specialized customer installations (projection mapping, custom computer vision pipelines, third-party hardware protocols, proprietary generative algorithms), external extensibility and robust production distribution are required.

We need:
1. A stable, versioned C ABI (`nf_plugin_abi.h`) and modern C++23 header-only SDK (`NodeForgePluginSDK.hpp`) allowing third parties to build dynamic operator plugins (`.dll`) without compiling against the entire NodeForge engine source tree or suffering MSVC C++ ABI fragility.
2. A multi-family dynamic operator runtime supporting:
   - `TexOp` GPU operators with direct access to Vulkan 1.3 `VkDevice`, input/output image handles, and push constants.
   - `ChanOp` control/audio operators with raw SIMD float arrays.
   - `DataOp` tabular/scripting operators with 2D string cell grids.
3. Multi-directory plugin auto-discovery (`<AppDir>/plugins`, `%APPDATA%/NodeForge/plugins`, project-local `./plugins`, and `NF_PLUGIN_PATH`) with zero-downtime hot-reloading and an integrated Studio UI Plugin Manager modal dialog.
4. A dedicated standalone kiosk player executable (`nodeforge_player.exe`) that executes `.nfp` projects in fullscreen/kiosk mode without ImGui docking/editor overhead for 24/7 permanent installations.
5. Production distribution packaging (CPack NSIS installer `.exe` with `.nfp`/`.nfc` file associations and portable `.zip`) along with an automated GitHub Actions CI/CD pipeline.

## Decisions

### 1. Dual-Layer Plugin Architecture (C ABI + C++23 Header-Only SDK)
- **Layer 1 (C ABI `nf_plugin_abi.h`)**:
  - Defined with strict `extern "C"` linkage, explicit struct member packing, and version handshake (`NF_PLUGIN_ABI_VERSION = 1`).
  - Uses vtables (`NF_NodeVTable`, `NF_TexOpVTable`, `NF_ChanOpVTable`, `NF_DataOpVTable`) with opaque plugin instance handles (`NF_PluginInstanceHandle`).
  - Allocations across the ABI boundary are managed via engine-provided allocators or explicit buffer copies to eliminate heap allocator mismatches.
- **Layer 2 (C++23 SDK `NodeForgePluginSDK.hpp`)**:
  - Header-only templates providing type-safe base classes: `nf::sdk::TexOpPlugin`, `nf::sdk::ChanOpPlugin`, `nf::sdk::DataOpPlugin`.
  - Automatic `try/catch` exception boundary barriers wrapping all virtual methods to guarantee third-party plugin exceptions never crash the host application.
  - Helper macro `NODEFORGE_PLUGIN_EXPORT` automating export entry points (`NF_GetPluginInfo`, `NF_RegisterOperators`, `NF_UnloadPlugin`).

### 2. Plugin Host Engine & Proxy Adapter (`PluginManager` & `PluginNodeProxy`)
- `PluginModule`: Win32 dynamic library wrapper loading DLLs with `LoadLibraryExA(..., LOAD_WITH_ALTERED_SEARCH_PATH)` to ensure private plugin dependencies are resolved from the plugin's folder.
- `PluginNodeProxy`: Generic `Node` subclass registered in `NodeRegistry::Instance()` that translates internal `CookContext`, `ParameterGroup`, and pin values to/from the C ABI descriptors.
- `PluginManager`: Central singleton managing discovery paths, caching loaded modules, hot-reloading DLLs by updating factory proxies in `NodeRegistry`, and triggering selective graph re-cooks.

### 3. Standalone Kiosk Player (`nodeforge_player.exe`)
- A streamlined, headless-first runtime that initializes GLFW + Vulkan 1.3 Swapchain + `Graph` runtime, completely skipping the ImGui dockspace, undo/redo stack, and canvas UI rendering.
- Supports command-line switches: `--project <path>`, `--fullscreen`, `--display <index>`, `--kiosk`, and `--fps <limit>`.
- Fully integrated with `DisplayManager` and `WarpBlendPass` for multi-projector activations.

### 4. Production Packaging & CI/CD
- CMake `CPack` is configured for:
  - NSIS installer: `NodeForge-Setup-v0.1.0-win64.exe` registering Start Menu entries, desktop shortcuts, and Windows Registry file associations for `.nfp` projects and `.nfc` components.
  - Portable bundle: `NodeForge-Portable-v0.1.0-win64.zip` containing self-contained binaries, DLL dependencies, Python runtime, shaders, and sample plugins.
  - Standalone SDK: `NodeForge-SDK-v0.1.0-win64.zip` containing public headers, CMake templates, and sample plugins.
- GitHub Actions CI/CD (`.github/workflows/ci.yml`) automating compilation, 110+ automated tests, and artifact publishing.

## Consequences
- Third-party developers can create custom operators using modern C++23 with zero NodeForge build dependencies.
- Plugin developers can iterate rapidly with live hot-reloading inside NodeForge Studio.
- Live shows can run uninterrupted with the ultra-lightweight `nodeforge_player.exe` kiosk engine.
- Complete installers and portable packages are generated automatically on every release.
