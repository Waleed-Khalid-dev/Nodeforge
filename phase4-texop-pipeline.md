# Phase 4 Plan — TexOp Pipeline (GPU Nodes)

## Context & Objectives
- **Target:** Phase 4 of [02-BUILD-ROADMAP-A-to-Z.md](file:///D:/[Project]/Touch%20Designer/02-BUILD-ROADMAP-A-to-Z.md).
- **Goal:** Build the real-time GPU texture operator pipeline in NodeForge. Deliver zero-copy GPU execution, dynamic Vulkan 1.3 rendering passes, lease-based render target pooling, shader hot-reloading, and the 10 foundational `TexOp` nodes.
- **Hardware Target:** NVIDIA RTX 3060 12GB (Vulkan 1.3, Dynamic Rendering, VMA).
- **Performance Budget:** 5-node chain @ 1080p < 4 ms GPU execution; zero memory leaks across 10,000 frames.

---

## Technical Decisions (Aligned via `/grill-me`)
1. **Shader Compilation Strategy:** Hybrid SPIR-V & Hot-Reload — precompiled embedded SPIR-V for fast, zero-dependency startup; runtime `shaderc` compilation and file watching for live (<200 ms) shader iteration in development mode.
2. **GPU Texture & Memory Management:** Lease-based `gpu::TexturePool` recycling `(width, height, format)` textures across frames to eliminate per-frame `VkImage` and `VmaAllocation` heap allocations.
3. **Execution Model:** Vulkan 1.3 `vkCmdBeginRendering` (Dynamic Rendering) with full-screen triangle raster passes (`gl_VertexIndex`) for filtering/sampling ops and compute shaders (`vkCmdDispatch`) for procedural noise generation.
4. **Scope:** Full Phase 4 delivery covering all 10 core TexOps, operator specs, ADR-0006, and comprehensive test suite.

---

## Work Breakdown Structure

### Phase 4.1 — Architecture & Specifications
- [ ] Write [ADR-0006: GPU TexOp Pipeline Architecture](file:///D:/[Project]/Touch%20Designer/docs/adr/ADR-0006-gpu-texop-pipeline.md).
- [ ] Write individual operator specs in `docs/operator-spec/`:
  - `NullTexOp.md`
  - `ConstantTexOp.md` (upgrade to GPU)
  - `NoiseTexOp.md`
  - `LoadImageTexOp.md`
  - `TransformTexOp.md` (upgrade to GPU)
  - `CompositeTexOp.md`
  - `BlurTexOp.md`
  - `LevelTexOp.md`
  - `ResolutionTexOp.md`
  - `ToWindowTexOp.md`

### Phase 4.2 — Core GPU Infrastructure (`src/gpu/` & `src/render/`)
- [ ] **Lease-based Texture Pool (`gpu::TexturePool`)**:
  - Pool textures by resolution, format, and sample count.
  - Generational lease tracking (`AcquireTexture`, `ReleaseTexture`, `GarbageCollect`).
  - Strict VMA memory reclamation without runtime memory leaks.
- [ ] **Shader Compiler & Hot-Reload Engine (`gpu::ShaderCompiler`)**:
  - Embed precompiled SPIR-V binaries for instant release startup.
  - Integrate `shaderc` GLSL compilation with file watchers for live updates.
- [ ] **Fullscreen & Compute Dispatchers (`gpu::FullscreenPass`, `gpu::ComputePass`)**:
  - Dynamic rendering raster pass (`vkCmdBeginRendering`) using fullscreen triangle vertex shader.
  - Compute pipeline executor for 2D procedural generators.

### Phase 4.3 — TexOp Engine Base (`src/operators/tex/TexOp.h` & `.cpp`)
- [ ] Abstract base class `TexOp` inheriting from `Node`:
  - Output texture pin management (`PinType::Tex`).
  - Output resolution and format negotiation.
  - Per-frame `RecordGpuPass(VkCommandBuffer cmd, const CookContext& ctx)`.
  - Zero-copy GPU memory chaining.

### Phase 4.4 — 10 Core TexOp Implementations
- [ ] **1. `Tex.Null` (`NullTexOp`)**: Pass-through operator forwarding upstream texture reference with zero copy overhead.
- [ ] **2. `Tex.Constant` (`ConstantTexOp`)**: Solid color RGBA generator rendering via fullscreen quad dynamic rendering.
- [ ] **3. `Tex.Noise` (`NoiseTexOp`)**: Procedural 2D Perlin / Simplex / Value noise with FBM octaves and animated time offset.
- [ ] **4. `Tex.LoadImage` (`LoadImageTexOp`)**: File loader utilizing `stb_image` with staging buffer upload to GPU texture.
- [ ] **5. `Tex.Transform` (`TransformTexOp`)**: 2D affine transformation (scale, rotate, translate, pivot, tile/repeat, wrap modes).
- [ ] **6. `Tex.Composite` (`CompositeTexOp`)**: Multi-mode dual-input blender (Over, Add, Multiply, Subtract, Screen, Darken, Lighten).
- [ ] **7. `Tex.Blur` (`BlurTexOp`)**: Two-pass separable Gaussian blur with configurable kernel size and blur radius.
- [ ] **8. `Tex.Level` (`LevelTexOp`)**: Photometric image adjustment (black point, white point, brightness, contrast, gamma).
- [ ] **9. `Tex.Resolution` (`ResolutionTexOp`)**: Resampler/resizer supporting Stretch, Fit Horizontal, Fit Vertical, and Center Crop.
- [ ] **10. `Tex.ToWindow` (`ToWindowTexOp`)**: Output display node presenting final texture directly to the GLFW Vulkan swapchain.

### Phase 4.5 — Testing & Quality Assurance
- [ ] **Unit Tests (`tests/unit/texop_test.cpp`)**:
  - Test individual output dimensions, parameter responsiveness, and GPU execution for each of the 10 TexOps.
- [ ] **End-to-End Integration Test (`tests/unit/texop_integration_test.cpp`)**:
  - Construct graph: `LoadImage` -> `Blur` -> `Composite` -> `Level`.
  - Read back single pixel and mathematically verify calculated color channels.
- [ ] **Performance & Leak Benchmark (`tests/benchmark/texop_benchmark.cpp`)**:
  - Run 10,000 frame cook loop to verify zero VMA / Vulkan descriptor leaks.
  - Benchmark 5-node chain @ 1080p ensuring < 4 ms total GPU execution time on RTX 3060.

---

## Definition of Done (DoD) Sign-Off Checklist
- [ ] All 10 `TexOp` operators implemented and registered in `NodeRegistry`.
- [ ] 5-node TexOp chain @ 1080p cooks in < 4 ms on RTX 3060.
- [ ] Zero GPU memory leaks over 10,000 frames verified via automated benchmark.
- [ ] Hot-reload modifies shader output dynamically in development mode.
- [ ] End-to-end image-filter-composite integration test passes with pixel math verification.
- [ ] Phase 4 signed off in `STATUS.md` and `02-BUILD-ROADMAP-A-to-Z.md`.
