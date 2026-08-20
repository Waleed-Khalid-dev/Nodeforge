# NodeForge — Phase 13: Plugin SDK, Packaging, Standalone Player & CI/CD Plan

## 📋 Overview

Phase 13 establishes NodeForge's external extensibility and production distribution capabilities. It delivers two foundational pillars:
1. **Dynamic Native Plugin SDK**: A versioned, stable C ABI + ergonomic modern C++23 header-only wrapper SDK that allows third-party developers to author and distribute custom high-performance operators (`TexOp` with Vulkan 1.3 GPU interop, `ChanOp` with SIMD channel processing, and `DataOp` with tabular data manipulation) as dynamic libraries (`.dll`). The runtime features multi-directory auto-discovery, live hot-reloading without editor restarts, and a dedicated Studio UI Plugin Manager.
2. **Production Packaging, Standalone Kiosk Player & CI/CD**: A dual-binary suite featuring the full Studio IDE (`nodeforge.exe`) and a lightweight, headless/fullscreen show engine (`nodeforge_player.exe`), bundled with embedded CPython 3, runtime dependencies, shaders, and assets into both an NSIS Windows installer (`.exe`) and a portable `.zip`. A complete GitHub Actions matrix CI/CD pipeline builds, tests, packages, and distributes both the application releases and a standalone `NodeForge-SDK-win64.zip`.

---

## 🎯 Project Type & Agent Assignments

- **Project Type**: Desktop System / Creative Coding Engine (C++23 / Vulkan 1.3 / Win32 / CPython 3 / CMake / NSIS)
- **Primary Agents**:
  - `project-planner`: Architecture, SDK interfaces, task planning (`brainstorming`, `plan-writing`, `architecture`)
  - `backend-specialist`: C ABI, dynamic loader, plugin runtime, standalone player engine (`clean-code`, `powershell-windows`)
  - `frontend-specialist`: Plugin Manager Studio modal UI (`clean-code`)
  - `devops-engineer`: CPack NSIS installer, portable bundling, GitHub Actions CI/CD matrix (`deployment-procedures`)
  - `test-engineer`: Plugin lifecycle, hot-reload, ABI compatibility, and packaging tests (`testing-patterns`, `verify-changes`)

---

## 🏆 Success Criteria & Definition of Done (DoD)

- [x] **Stable C ABI & C++23 SDK**: `nf_plugin_abi.h` and `NodeForgePluginSDK.hpp` compile cleanly in both in-tree and external out-of-tree CMake projects without internal NodeForge source dependencies.
- [x] **Multi-Family Dynamic Operators**:
  - `TexOp` Plugin: Accesses Vulkan 1.3 device, descriptors, and input/output image handles for GPU shader passes.
  - `ChanOp` Plugin: Manipulates raw SIMD float arrays and dynamic channel names at audio/control sample rates.
  - `DataOp` Plugin: Inspects, transforms, and produces 2D string cell tables.
- [x] **Plugin Manager & Discovery**: Auto-scans `<AppDir>/plugins/`, `%APPDATA%/NodeForge/plugins/`, project-local `./plugins/`, and `NF_PLUGIN_PATH`; loads and registers operators dynamically into `NodeRegistry`; provides live hot-reload API and Studio UI dialog.
- [x] **Three Working Sample Plugins**:
  - `Tex.InvertPlugin`: GPU Vulkan compute/raster shader operator.
  - `Chan.HarmonicLFOPlugin`: Multi-harmonic procedural channel generator.
  - `Data.CSVTransformPlugin`: High-throughput tabular text transformer.
- [x] **Standalone Kiosk Player (`nodeforge_player.exe`)**:
  - Lightweight binary executing without docking UI overhead.
  - CLI flags: `--project <path>`, `--fullscreen`, `--display <index>`, `--kiosk`, `--fps <limit>`.
  - Integrates with `DisplayManager` and `WarpBlendPass` for multi-projector activations.
- [x] **Packaging & Installer**:
  - CMake CPack generates `NodeForge-Setup-v0.1.0-win64.exe` (NSIS with `.nfp`/`.nfc` file associations & Start Menu shortcuts).
  - Generates self-contained `NodeForge-Portable-v0.1.0-win64.zip` portable distribution including all runtime DLLs, Python environment, shaders, and fonts.
  - Generates `NodeForge-SDK-v0.1.0-win64.zip` containing headers, stub libs, and example CMake projects.
- [x] **Automated CI/CD Workflow**: GitHub Actions `.github/workflows/ci.yml` runs full build, all 105+ automated tests, and publishes release artifacts.
- [x] **Zero Memory Leaks & 100% Test Pass Rate**: Full test suite across plugin loading, ABI safety, hot-reloading, and player runtime.

---

## 🛠️ Tech Stack & Approved Libraries

| Layer | Component | Choice / Specification |
|---|---|---|
| **Plugin C ABI** | Dynamic Interface | Versioned `extern "C"` ABI (`nf_plugin_abi.h`), ABI version handshake (`NF_PLUGIN_ABI_VERSION = 1`) |
| **Plugin C++ SDK** | Developer Wrapper | C++23 header-only templates (`NodeForgePluginSDK.hpp`), RAII wrappers, exception boundary guards |
| **Dynamic Loading** | Win32 Shared Libs | `LoadLibraryExA`, `GetProcAddress`, `FreeLibrary`, DLL dependency isolation |
| **Vulkan GPU Interop** | GPU Node Hooks | Vulkan 1.3 `VkDevice`, `VkCommandBuffer`, `VkImageView`, `VkFormat`, push constant interfaces |
| **Packaging** | Installer & Bundle | CMake `CPack`, NSIS (`makensis`), PowerShell distribution staging scripts |
| **Standalone Engine** | Kiosk Runtime | `nodeforge_player` (GLFW + Vulkan 1.3 Swapchain + Graph Runtime, zero ImGui canvas overhead) |
| **CI/CD** | Automation | GitHub Actions (Windows Server 2022 runner, MSVC 2022, Ninja, vcpkg cache) |

---

## 📁 File Structure & New Files

```plaintext
Touch Designer/
├── .github/
│   └── workflows/
│       └── ci.yml                             # [NEW] GitHub Actions CI/CD matrix build & test
├── sdk/
│   ├── include/
│   │   ├── nf_plugin_abi.h                    # [NEW] Stable C ABI header (ABI versioning, C types, vtables)
│   │   └── NodeForgePluginSDK.hpp             # [NEW] Modern C++23 header-only SDK wrappers
│   ├── samples/
│   │   ├── TexInvertPlugin/                   # [NEW] Sample GPU TexOp plugin
│   │   │   ├── CMakeLists.txt
│   │   │   ├── TexInvertPlugin.cpp
│   │   │   └── invert.comp.glsl
│   │   ├── ChanHarmonicLFOPlugin/             # [NEW] Sample SIMD ChanOp plugin
│   │   │   ├── CMakeLists.txt
│   │   │   └── ChanHarmonicLFOPlugin.cpp
│   │   └── DataCSVTransformPlugin/            # [NEW] Sample DataOp table plugin
│   │       ├── CMakeLists.txt
│   │       └── DataCSVTransformPlugin.cpp
│   └── README.md                              # [NEW] Plugin developer guide & CMake instructions
├── src/
│   ├── plugin/
│   │   ├── PluginTypes.h                      # [NEW] Internal C++ plugin descriptor structs
│   │   ├── PluginModule.h                     # [NEW] Win32 DLL wrapper & entrypoint binder
│   │   ├── PluginModule.cpp                   # [NEW] DLL load/unload & symbol resolution
│   │   ├── PluginManager.h                    # [NEW] Multi-directory scanner, registry & hot-reloader
│   │   ├── PluginManager.cpp                  # [NEW] Discovery search paths & node factory binding
│   │   ├── PluginNodeProxy.h                  # [NEW] Graph Node adapter proxying C ABI vtables
│   │   └── PluginNodeProxy.cpp                # [NEW] Cook dispatch, param translation, exception isolation
│   ├── player/
│   │   ├── PlayerApp.h                        # [NEW] Lightweight standalone player engine
│   │   ├── PlayerApp.cpp                      # [NEW] Headless/fullscreen project loader & loop
│   │   └── main_player.cpp                    # [NEW] Entry point for nodeforge_player.exe
│   ├── ui/
│   │   └── panels/
│   │       ├── PluginManagerModal.h           # [NEW] Studio UI Plugin Manager modal dialog
│   │       └── PluginManagerModal.cpp         # [NEW] Plugin inspection, status badges, live reload buttons
│   └── ui/
│       └── MainMenuBar.cpp                    # [MODIFY] Add "Plugins -> Plugin Manager..." menu item
├── package/
│   ├── nsis/
│   │   ├── installer.nsi.in                   # [NEW] Custom NSIS installer template with file associations
│   │   └── nodeforge.ico                      # [NEW] NodeForge application and file icons
│   └── scripts/
│       └── bundle_portable.ps1                # [NEW] PowerShell bundle staging script (DLLs, Python, Shaders)
├── docs/
│   ├── adr/
│   │   └── ADR-0013-plugin-sdk-and-packaging.md # [NEW] ADR for Plugin SDK & Distribution Architecture
│   └── operator-spec/
│       └── PluginProxyNode.md                 # [NEW] Operator spec for dynamic plugin bridge
├── tests/
│   ├── plugin_abi_test.cpp                    # [NEW] C ABI struct layouts, version handshake, vtable tests
│   ├── plugin_manager_test.cpp                # [NEW] Discovery, load, cook, hot-reload, error recovery tests
│   └── player_runtime_test.cpp                # [NEW] Standalone player CLI parsing, headless project cook tests
└── CMakeLists.txt                             # [MODIFY] Add SDK install targets, CPack config, player binary, plugin tests
```

---

## 📋 Task Breakdown & Implementation Phases

### Phase 13.1: Stable C ABI & Modern C++23 Header-Only SDK
- **Task 13.1.1**: Author `sdk/include/nf_plugin_abi.h` defining the strict C ABI:
  - `NF_PluginInfo`, `NF_PluginVersion`, `NF_PluginABI_Version`
  - Function pointer tables: `NF_NodeVTable`, `NF_TexOpVTable`, `NF_ChanOpVTable`, `NF_DataOpVTable`
  - Parameter descriptors, Pin definitions, and Vulkan GPU texture handles (`NF_VulkanTextureDescriptor`, `NF_ChannelBufferDescriptor`, `NF_DataTableDescriptor`)
  - Exported entrypoint signatures: `NF_GetPluginInfo()`, `NF_RegisterOperators()`, `NF_UnloadPlugin()`
  - *Agent*: `backend-specialist` | *Skills*: `clean-code`
  - *INPUT*: Architecture specs -> *OUTPUT*: `sdk/include/nf_plugin_abi.h` -> *VERIFY*: Clean compilation in pure C and C++ modes.
- **Task 13.1.2**: Author `sdk/include/NodeForgePluginSDK.hpp` providing ergonomic C++23 templates:
  - `nf::sdk::PluginBase`, `nf::sdk::TexOpPlugin`, `nf::sdk::ChanOpPlugin`, `nf::sdk::DataOpPlugin`
  - Type-safe parameter getters/setters, Pin value wrappers, Vulkan helper macros
  - Strict exception boundary catches preventing DLL crashes from taking down the host engine
  - Macro helper `NODEFORGE_PLUGIN_EXPORT` for effortless single-line registration
  - *Agent*: `backend-specialist` | *Skills*: `clean-code`
  - *INPUT*: `nf_plugin_abi.h` -> *OUTPUT*: `NodeForgePluginSDK.hpp` -> *VERIFY*: Header test compiles without warnings under `/W4 /WX`.

### Phase 13.2: Plugin Engine & Dynamic Operator Proxy
- **Task 13.2.1**: Implement `src/plugin/PluginModule.h` and `src/plugin/PluginModule.cpp`:
  - Win32 DLL loader using `LoadLibraryExA(path, NULL, LOAD_WITH_ALTERED_SEARCH_PATH)`
  - Safe symbol resolution for mandatory entrypoints (`NF_GetPluginInfo`, `NF_RegisterOperators`, `NF_UnloadPlugin`)
  - ABI version handshake and compatibility validation
  - *Agent*: `backend-specialist` | *Skills*: `clean-code`
  - *INPUT*: Win32 API -> *OUTPUT*: `PluginModule` class -> *VERIFY*: Unit test loading test mock DLLs.
- **Task 13.2.2**: Implement `src/plugin/PluginNodeProxy.h` and `src/plugin/PluginNodeProxy.cpp`:
  - Generic `Node` subclass adapting plugin vtables into NodeForge's graph cook pipeline
  - Specializations for `PluginTexOpProxy`, `PluginChanOpProxy`, and `PluginDataOpProxy`
  - Marshals `CookContext`, parameter values, and data structures across the ABI boundary
  - Full exception safety with diagnostic error reporting
  - *Agent*: `backend-specialist` | *Skills*: `clean-code`
  - *INPUT*: `Node.h` + `nf_plugin_abi.h` -> *OUTPUT*: `PluginNodeProxy` -> *VERIFY*: Proxy correctly forwards parameters and cook calls.
- **Task 13.2.3**: Implement `src/plugin/PluginManager.h` and `src/plugin/PluginManager.cpp`:
  - Singleton manager managing loaded modules and registered dynamic node types
  - Search path scanning: `<AppDir>/plugins/`, `%APPDATA%/NodeForge/plugins/`, project `./plugins/`, `NF_PLUGIN_PATH`
  - Registration of plugin operators directly into `NodeRegistry::Instance()` with custom category and family
  - Thread-safe reload mechanism allowing hot swapping of updated DLLs during runtime
  - *Agent*: `backend-specialist` | *Skills*: `clean-code`
  - *INPUT*: `NodeRegistry.h` -> *OUTPUT*: `PluginManager` -> *VERIFY*: `plugin_manager_test.cpp` passes all discovery & reload tests.

### Phase 13.3: Sample Operator Plugins & Developer SDK Template
- **Task 13.3.1**: Build `sdk/samples/TexInvertPlugin`:
  - GPU TexOp plugin demonstrating dynamic texture processing (color inversion & gamma)
  - Contains embedded SPIR-V / runtime shader compilation demonstration
  - Standalone `CMakeLists.txt` configured to build with only the SDK headers
  - *Agent*: `backend-specialist` | *Skills*: `clean-code`
  - *INPUT*: SDK headers -> *OUTPUT*: `TexInvertPlugin.dll` -> *VERIFY*: Builds and cooks in NodeForge graph.
- **Task 13.3.2**: Build `sdk/samples/ChanHarmonicLFOPlugin`:
  - SIMD ChanOp plugin generating multi-harmonic waveforms (fundamental, 3rd, 5th harmonics)
  - Demonstrates channel creation, parameter binding, and real-time buffer generation
  - *Agent*: `backend-specialist` | *Skills*: `clean-code`
  - *INPUT*: SDK headers -> *OUTPUT*: `ChanHarmonicLFOPlugin.dll` -> *VERIFY*: Builds and plots multi-harmonic channels.
- **Task 13.3.3**: Build `sdk/samples/DataCSVTransformPlugin`:
  - DataOp plugin parsing, sorting, and filtering 2D DataTable grids
  - *Agent*: `backend-specialist` | *Skills*: `clean-code`
  - *INPUT*: SDK headers -> *OUTPUT*: `DataCSVTransformPlugin.dll` -> *VERIFY*: Builds and transforms test table data.
- **Task 13.3.4**: Create `sdk/README.md` and SDK packaging CMake targets generating `NodeForge-SDK-win64.zip`.
  - *Agent*: `devops-engineer` | *Skills*: `documentation-templates`

### Phase 13.4: Studio UI Plugin Manager Modal
- **Task 13.4.1**: Implement `src/ui/panels/PluginManagerModal.h` and `.cpp`:
  - ImGui modal dialog displaying:
    - Table of loaded plugins (Name, Version, Author, Description, DLL Path, Loaded Operators count)
    - Active Search Paths list with "+ Add Directory" button
    - Status badges (Active, Outdated ABI, Error)
    - "Reload Plugin", "Reload All", and "Open Plugin Folder" actions
  - *Agent*: `frontend-specialist` | *Skills*: `clean-code`
  - *INPUT*: `PluginManager.h` -> *OUTPUT*: `PluginManagerModal` -> *VERIFY*: Dialog opens from MainMenuBar and hot-reloads DLLs live.
- **Task 13.4.2**: Integrate Plugin Manager into `src/ui/MainMenuBar.cpp` under menu `Plugins -> Plugin Manager...` (Shortcut: Ctrl+Shift+P).
  - *Agent*: `frontend-specialist` | *Skills*: `clean-code`

### Phase 13.5: Standalone Kiosk Player (`nodeforge_player.exe`)
- **Task 13.5.1**: Implement `src/player/PlayerApp.h` and `src/player/PlayerApp.cpp`:
  - Ultra-lightweight runtime container bypassing ImGui editor canvas, panels, and undo stack
  - CLI argument parser supporting:
    - `--project <path>`: Project `.nfp` file to load and execute
    - `--fullscreen` / `--windowed`: Display presentation mode
    - `--display <index>`: Physical monitor targeting
    - `--kiosk`: Lock mouse cursor and prevent accidental exit
    - `--fps <rate>`: Target framerate cap (e.g. 60 or 120)
  - Real-time evaluation loop driving `Graph::Cook()`, `DisplayManager`, and `ProjectorOutTexOp`
  - *Agent*: `backend-specialist` | *Skills*: `clean-code`
  - *INPUT*: `nodeforge_core` -> *OUTPUT*: `PlayerApp` engine -> *VERIFY*: `player_runtime_test.cpp` runs headless project loop.
- **Task 13.5.2**: Add `add_executable(nodeforge_player src/player/main_player.cpp)` to root `CMakeLists.txt`.
  - *Agent*: `backend-specialist` | *Skills*: `clean-code`

### Phase 13.6: Packaging, CPack NSIS Installer & CI/CD Pipeline
- **Task 13.6.1**: Implement CMake install targets and CPack configuration in `CMakeLists.txt`:
  - Installs binaries (`nodeforge.exe`, `nodeforge_player.exe`), core DLLs, plugins, SDK, Python runtime, shaders, and icons.
  - NSIS configuration: `NodeForge-Setup-v0.1.0-win64.exe` with desktop shortcut, Start Menu folder, uninstaller, and file association for `.nfp` (NodeForge Project) and `.nfc` (NodeForge Component).
  - Portable ZIP configuration: `NodeForge-Portable-v0.1.0-win64.zip`.
  - *Agent*: `devops-engineer` | *Skills*: `deployment-procedures`
  - *INPUT*: CMake build targets -> *OUTPUT*: Working CPack installer & ZIP generator -> *VERIFY*: CPack produces valid installer and ZIP.
- **Task 13.6.2**: Implement PowerShell distribution bundler script `package/scripts/bundle_portable.ps1`:
  - Automatically stages executable, dependencies (`glfw3.dll`, `shaderc_shared.dll`, `vulkan-1.dll`), CPython 3 runtime directory, default shaders, and sample plugins into a standalone folder.
  - *Agent*: `devops-engineer` | *Skills*: `powershell-windows`
- **Task 13.6.3**: Implement GitHub Actions automated CI/CD pipeline `.github/workflows/ci.yml`:
  - Windows Server 2022 runner matrix build
  - Installs vcpkg dependencies with binary caching
  - Builds Release configuration of `nodeforge`, `nodeforge_player`, sample plugins, and tests
  - Executes full test suite (`ctest --output-on-failure`)
  - Generates and uploads release artifacts:
    1. `NodeForge-Setup-win64.exe`
    2. `NodeForge-Portable-win64.zip`
    3. `NodeForge-SDK-win64.zip`
  - *Agent*: `devops-engineer` | *Skills*: `deployment-procedures`

### Phase 13.7: Documentation, Architecture & Operator Specs
- **Task 13.7.1**: Write `docs/adr/ADR-0013-plugin-sdk-and-packaging.md` documenting ABI design, plugin lifecycle, security boundaries, and packaging structure.
- **Task 13.7.2**: Write `docs/operator-spec/PluginProxyNode.md` specifying dynamic node proxy lifecycle, pin marshaling, and GPU resource sharing.
- **Task 13.7.3**: Update `STATUS.md` and `02-BUILD-ROADMAP-A-to-Z.md` with completed Phase 13 DoD checklist and milestones.

---

## 🧪 Phase X: Final Verification Plan

### Automated Test Suite Execution
- Run comprehensive CTest across all 41 test suites:
  ```powershell
  ctest --test-dir build --output-on-failure -C Release
  ```
- Specific Phase 13 tests:
  - `plugin_abi_test`: Verifies C ABI struct alignment, size guarantees, and version handshake.
  - `plugin_manager_test`: Verifies dynamic discovery, loading sample DLLs, cook execution, parameter passing, and hot-reload.
  - `player_runtime_test`: Verifies standalone player CLI options, headless project loading, and multi-frame cook execution.

### Manual & Staging Verification
1. **Out-of-Tree Plugin Build**: Build `TexInvertPlugin` using only the installed SDK headers against a separate build folder.
2. **Hot-Reload Validation**: In NodeForge Studio, place a `Tex.InvertPlugin` node, modify the plugin DLL on disk, click "Reload Plugin" in the Plugin Manager, and verify that the canvas node updates without restarting.
3. **Kiosk Player Launch**: Launch `nodeforge_player.exe --project samples/projection_test.nfp --fullscreen` and verify 60 FPS presentation.
4. **Installer Test**: Run `NodeForge-Setup.exe`, verify file associations `.nfp` open in NodeForge, and test clean uninstallation.

---

## 📌 Agent Execution Summary

| Phase | Description | Primary Agent | Key Skills |
|---|---|---|---|
| **13.1** | C ABI & Modern C++23 SDK | `backend-specialist` | `clean-code`, `architecture` |
| **13.2** | Plugin Module Loader & Proxy Engine | `backend-specialist` | `clean-code` |
| **13.3** | 3 Sample Plugins (Tex, Chan, Data) | `backend-specialist` | `clean-code` |
| **13.4** | Studio UI Plugin Manager Modal | `frontend-specialist` | `clean-code` |
| **13.5** | Standalone Kiosk Player (`nodeforge_player`) | `backend-specialist` | `clean-code` |
| **13.6** | CPack Installer, Portable Bundle & CI/CD | `devops-engineer` | `deployment-procedures`, `powershell-windows` |
| **13.7** | ADR-0013, Operator Specs & Status Update | `project-planner` | `plan-writing`, `documentation-templates` |
| **X** | Final Verification & Test Sign-off | `test-engineer` | `testing-patterns`, `verify-changes` |
