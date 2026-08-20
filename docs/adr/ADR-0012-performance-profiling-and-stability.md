# ADR-0012: Performance Profiling, Stability, and Crash Recovery Architecture

## Status
Accepted

## Context
As NodeForge enters production readiness for large-scale multi-projector activations (e.g. 5760x1080 canvas across multiple outputs, 60 FPS target), live show reliability is paramount. Performance bottlenecks, memory leaks, GPU timeouts, and unexpected runtime faults must be immediately observable, diagnosable, and recoverable.

We need:
1. Sub-microsecond CPU cook time measurement per node with rolling statistical metrics (min, max, avg, last, call count).
2. Accurate Vulkan GPU execution timestamps (`VkQueryPool`) for individual render passes and compute passes.
3. Real-time TexturePool memory watermarking, active lease tracking, and leak alert triggers.
4. Integrated Studio UI visualizations: a dockable `ProfilerPanel`, live cook time badges under node titles on the infinite canvas, and a floating `PerformanceHUD` (F3 overlay).
5. Robust stability guarantees: simulated GPU device loss (`VK_ERROR_DEVICE_LOST`) recovery, frame pacing governance, and Win32 Structured Exception Handling (SEH) with emergency project crash state snapshotting (`.nfp.crash`).

## Decisions

### 1. Dual-Engine Profiler (`CookProfiler` & `GpuTimerPool`)
- `CookProfiler`: A singleton registry recording execution durations for each node during `Graph::CookNode()`. It maintains a lock-free or lightweight mutex-guarded rolling history ring buffer (last 120 frames) to compute minimum, maximum, average, and instantaneous cook durations in milliseconds.
- `GpuTimerPool`: Manages Vulkan timestamp query pools (`VK_QUERY_TYPE_TIMESTAMP`) allocated on the graphics/compute device. It issues `vkCmdWriteTimestamp` at pass start and end, retrieves results via non-blocking `vkGetQueryPoolResults`, and converts GPU clock ticks to milliseconds using `VkPhysicalDeviceLimits::timestampPeriod`.

### 2. TexturePool Leak Detector & VRAM Accounting
- `TexturePool` tracks:
  - Total leased textures currently in flight.
  - Peak concurrent allocation count (high-water mark).
  - Allocation vs. cache reuse ratio.
  - Per-operator attribution (which node requested the lease).
- A lease watchdog generates warning logs if any leased texture remains unreturned across multiple frames.

### 3. Studio UI & HUD Integration
- `ProfilerPanel`: An ImGui dockable panel providing a searchable, sortable table of all active nodes (Name, Type, Family, CPU ms, GPU ms, Dirty Rate), a hierarchical flame-graph, and a memory timeline sparkline.
- Canvas Badges: Dynamic badges rendered directly on canvas nodes showing cook latency with color-coded warning tiers (<1ms Green, 1-4ms Yellow, >4ms Red).
- `PerformanceHUD`: Translucent floating overlay toggled with **F3** displaying real-time FPS, CPU Graph Cook vs. GPU Submit latency breakdown, VRAM usage, and frame pacing diagnostics.

### 4. Crash Recovery & Emergency Snapshot (`CrashReporter`)
- A top-level Win32 unhandled exception filter (`SetUnhandledExceptionFilter`) catches access violations or unhandled errors, writes a diagnostic crash report with memory and system state, and serializes the in-memory graph to `.nfp.crash` before termination.

## Consequences
- Developers and artists can instantly identify slow nodes on the canvas.
- Memory leaks in complex generative pipelines are caught within frames instead of causing out-of-memory crashes hours later.
- Emergency crash autosaves ensure work is never lost in unexpected hardware faults.
