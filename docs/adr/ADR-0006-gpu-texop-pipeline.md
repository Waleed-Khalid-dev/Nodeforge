# ADR-0006: GPU TexOp Pipeline and Real-time Texture Engine

## Status
Accepted (Phase 4)

## Context
In NodeForge, the Texture Operator (`TexOp` / TOP) family is responsible for high-performance 2D image processing, procedural generation, video compositing, and presentation. Real-time media playback and projection mapping (e.g. 5760x1080 @ 60/120fps) require sub-millisecond execution times per operator without CPU-GPU readback bottlenecks or frame-to-frame heap allocation jitter.

Vulkan 1.3 provides `VK_KHR_dynamic_rendering` and synchronization2, eliminating legacy Vulkan render pass/framebuffer boilerplate and enabling zero-copy GPU-to-GPU dataflow between nodes.

## Decisions

### 1. Zero-Copy GPU Dataflow
- Data flowing across `PinType::Tex` pins is represented by lightweight GPU texture handles (`std::shared_ptr<gpu::Texture2D>` or pooled references) rather than CPU byte buffers.
- An upstream `TexOp` writes directly to a GPU render target or storage image; downstream nodes bind that texture directly as a sampled descriptor (`VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER`).
- Zero CPU readback occurs during normal graph cooking. CPU readback is only triggered on explicit request (e.g. test harnesses, pixel inspectors, or CPU image saving).

### 2. Lease-Based Render Target Pooling (`gpu::TexturePool`)
- Allocating `VkImage` and `VmaAllocation` per frame causes fatal GPU memory fragmentation and driver stalls.
- A centralized `gpu::TexturePool` leases texture instances keyed by `(width, height, format, usageFlags)`.
- Nodes acquire render targets during `Cook()` and release leases at the end of each frame.
- Unreferenced textures are cached across frames and recycled with generational eviction policy.

### 3. Vulkan 1.3 Dynamic Rendering & Execution Model
- Raster-based image filters (`Blur`, `Transform`, `Composite`, `Level`, `Resolution`, `Constant`) use `vkCmdBeginRendering` with a single full-screen triangle generated analytically in the vertex shader (`gl_VertexIndex` 0, 1, 2). No vertex buffers or index buffers are required.
- Procedural generators (`Noise`) utilize compute shaders (`vkCmdDispatch`) writing to `image2D` storage images.
- All command recordings append to the per-frame `VkCommandBuffer` in topological execution order with memory barriers (`vkCmdPipelineBarrier2`).

### 4. Hybrid Shader Strategy & Live Hot-Reload
- Embedded SPIR-V bytecode is packaged with the engine for instant, zero-dependency release startup.
- In development mode, `gpu::ShaderCompiler` uses `shaderc` to compile GLSL on the fly with a filesystem watcher, enabling sub-200ms live shader hot-reloading without application restarts.

### 5. 10 Foundational TexOp Nodes
The initial suite implements:
1. `NullTexOp`: Zero-copy pass-through alias.
2. `ConstantTexOp`: Solid color / RGBA generator with dynamic rendering.
3. `NoiseTexOp`: Procedural 2D Perlin / Simplex / FBM compute shader.
4. `LoadImageTexOp`: File loading via `stb_image` with staging buffer upload.
5. `TransformTexOp`: Affine UV transform (translate, rotate, scale, pivot, tile/wrap).
6. `CompositeTexOp`: Dual-input blend (Over, Add, Multiply, Subtract, Screen, Darken, Lighten).
7. `BlurTexOp`: Two-pass separable Gaussian blur.
8. `LevelTexOp`: Photometric color adjustments (black/white levels, gamma, contrast, brightness).
9. `ResolutionTexOp`: Aspect-ratio aware resampler (Stretch, Fit H, Fit V, Crop).
10. `ToWindowTexOp`: Direct swapchain presentation blit.

## Consequences
- **Positive:** Maximum rendering throughput (<4ms for 5-node 1080p pipeline on RTX 3060); zero VMA memory leaks; clean Vulkan 1.3 Dynamic Rendering architecture.
- **Trade-offs:** GPU driver requirements strictly Vulkan 1.3+; compute and raster passes must carefully synchronize layout transitions using pipeline barriers.
