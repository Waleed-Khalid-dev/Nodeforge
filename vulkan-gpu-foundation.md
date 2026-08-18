# Vulkan GPU Foundation (Phase 1)

## Overview
This phase establishes the core GPU foundation for NodeForge using Vulkan. The goal is to initialize the graphics engine, handle memory via VMA, and successfully upload and display a PNG image using a fullscreen triangle shader. No UI or graph logic is built yet.

## Project Type
BACKEND / C++ ENGINE (Native Windows Desktop)

## Success Criteria
- The application opens a GLFW window backed by Vulkan.
- A loaded PNG image is displayed correctly.
- Window resizing works without crashing.
- Vulkan Validation layers run clean on the happy path.
- Tracy profiler captures frame rendering times.

## Tech Stack
- **Vulkan** (via `vk-bootstrap` for safe initialization)
- **VMA** (Vulkan Memory Allocator for GPU memory)
- **GLFW** (Windowing and Input)
- **stb_image** (Image loading)
- **Tracy** (Frame profiling)
- **GoogleTest** (Smoke testing)

## File Structure
```text
src/gpu/
├── Device.h / Device.cpp
├── Swapchain.h / Swapchain.cpp
├── Texture.h / Texture.cpp
├── FrameResources.h / FrameResources.cpp
src/app/
├── main.cpp
shaders/
├── fullscreen.vert
├── texture.frag
```

## Task Breakdown

| Task | Agent | Skill | Description | Verification (INPUT → OUTPUT → VERIFY) |
|------|-------|-------|-------------|-------------------------------------------|
| 1.1 `gpu::Device` | `backend-specialist` | `cpp-patterns` | Initialize Vulkan instance, physical device, logical device, queues using `vk-bootstrap`. Add validation layer support. | Init settings → Valid Vulkan device → Validation layers active and clean. |
| 1.2 `VMA` Setup | `backend-specialist` | `cpp-patterns` | Integrate Vulkan Memory Allocator with the logical device. | Vulkan Device → VmaAllocator → Allocator creation succeeds. |
| 1.3 `gpu::Swapchain` | `backend-specialist` | `cpp-patterns` | Create swapchain and handle window resize recreation. | GLFW Window → Swapchain images → Resize triggers clean recreation without leak. |
| 1.4 Frame Resources | `backend-specialist` | `cpp-patterns` | Create per-frame command buffers, semaphores, fences. | Frame index → Sync objects → Frames render without CPU/GPU deadlocks. |
| 1.5 Shaders & Pipeline | `backend-specialist` | `cpp-patterns` | Write `fullscreen.vert` and `texture.frag`. Compile to SPIR-V and load into a graphics pipeline. | GLSL source → Pipeline object → Draws a fullscreen triangle. |
| 1.6 `gpu::Texture2D` | `backend-specialist` | `cpp-patterns` | Use `stb_image` to load PNG, allocate with VMA, and copy via staging buffer. | PNG File → Vulkan Image → Pixels match source image. |
| 1.7 Tracy Integration | `backend-specialist` | `performance-profiling` | Add Tracy zones around frame presentation and device setup. | Source code → Tracy server connection → Frame data visible in profiler. |
| 1.8 Smoke Test | `test-engineer` | `testing-patterns` | Write GoogleTest to create a headless `gpu::Device`. | Source code → Test binary → Tests pass without crashing. |

## Phase X: Verification
- [ ] `cmake --build` succeeds without warnings on core.
- [ ] Run application: Displays test PNG.
- [ ] Resize application: Does not crash.
- [ ] Validation layers: 0 errors in debug console.
- [ ] GoogleTest: `ctest` passes `DeviceCreationTest`.
