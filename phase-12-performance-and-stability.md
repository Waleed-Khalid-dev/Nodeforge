# Phase 12 Plan — Performance, Stability, Profiling

**Goal:** Implement industrial-grade performance measurement, GPU query pool pass timing, real-time Profiler UI & Performance HUD, TexturePool memory leak detection, automated soak test harness with device loss recovery, and emergency crash dumping.

---

## 1. Architectural Foundations

- **ADR Required:** `docs/adr/ADR-0012-performance-profiling-and-stability.md`
- **Core Engine Modules (`src/profiling/`, `src/gpu/`, `src/diagnostics/`):**
  1. `CookProfiler`: High-precision per-node CPU execution timer (`std::chrono::high_resolution_clock`), cumulative cook statistics (min, max, avg, last ms), and call-count tracking.
  2. `GpuTimerPool`: Vulkan 1.3 `VkQueryPool` timestamp manager wrapping `vkCmdWriteTimestamp` around render passes and compute dispatches, resolving GPU execution time in milliseconds via `vkGetQueryPoolResults`.
  3. `TexturePoolLeakDetector`: Watermark tracker recording peak allocation counts, active leases vs. pooled textures, and per-operator VRAM byte accounting.
  4. `FramePacer`: Frame timing, vsync jitter analyzer, dropped frame detection, and target frame rate governor (30, 60, 120, Unlimited).
  5. `CrashReporter`: Win32 Structured Exception Handling (SEH) unhandled exception filter capturing crash dump diagnostics and auto-saving emergency recovery projects (`.nfp.crash`).

---

## 2. Studio UI Integration (`src/ui/`)

1. **`ProfilerPanel`:**
   - Dockable ImGui panel featuring:
     - Sortable node performance table (Node Name, Family, CPU Cook ms, GPU Pass ms, Dirty Count).
     - Live execution flame-graph / hierarchical timeline.
     - VRAM allocation history & TexturePool leak watermark sparkline graph.
2. **Canvas Node Badges:**
   - Dynamic cook time badges (`0.04 ms`) directly beneath node headers on the canvas with color-coded warning thresholds (Green < 1ms, Yellow 1–4ms, Red > 4ms).
3. **Floating Performance HUD (F3):**
   - Translucent top-right overlay displaying:
     - Real-time FPS & frame delta graph.
     - CPU Graph Cook time vs. GPU Present latency breakdown.
     - Active GPU VRAM allocation & TexturePool lease health.
     - Frame drop counter & vsync stability.

---

## 3. Tasks Breakdown

### Phase 12.1: Performance Engine & Profiling Core
- [ ] Implement `src/profiling/CookProfiler.h` & `.cpp` with thread-safe sample recording and rolling window metrics.
- [ ] Implement `src/gpu/GpuTimerPool.h` & `.cpp` using Vulkan timestamp queries with calibration against CPU timestamps.
- [ ] Integrate profiling hooks into `nf::Graph::CookNode()` and `nf::FullscreenPass` / `nf::ComputePass`.

### Phase 12.2: Memory Watermarking & Leak Detection
- [ ] Extend `TexturePool` with leak detection hooks, lease tracking metadata, and per-node allocation attribution.
- [ ] Add active lease timeout / unreleased lease detection with spdlog warnings.

### Phase 12.3: UI Profiler Panel & Performance HUD
- [ ] Implement `src/ui/ProfilerPanel.h` & `.cpp` with search filtering, sortable metrics table, and sparklines.
- [ ] Implement `src/ui/PerformanceHUD.h` & `.cpp` with F3 toggle shortcut and frame pacing analytics.
- [ ] Update `CanvasPanel` to render per-node cook time badges.
- [ ] Register new panels in `EditorApp` menu bar under `Window -> Profiler` and `Window -> Performance HUD`.

### Phase 12.4: Stability, Fault Injection & Crash Guard
- [ ] Implement `src/diagnostics/CrashReporter.h` & `.cpp` with `SetUnhandledExceptionFilter` and emergency project snapshot writer.
- [ ] Implement simulated GPU device loss (`VK_ERROR_DEVICE_LOST`) recovery handler and pipeline reset routine.

### Phase 12.5: Automated Test Suite & Benchmarks
- [ ] Author `tests/unit/profiler_test.cpp` (CPU timer accuracy, GPU query calibration, rolling averages).
- [ ] Author `tests/unit/leak_detector_test.cpp` (TexturePool allocation/release balance, unreleased lease detection).
- [ ] Author `tests/unit/crash_recovery_test.cpp` (Crash snapshot serialization & state preservation).
- [ ] Author `tests/benchmark/long_soak_benchmark.cpp` (Multi-thousand frame complex stress test with <0.01MB memory delta gate).

---

## 4. Definition of Done (DoD)

- [ ] All unit tests and benchmarks pass with 100% success rate.
- [ ] `CookProfiler` measures per-node CPU execution times with sub-microsecond resolution.
- [ ] `GpuTimerPool` measures GPU passes via Vulkan timestamp queries.
- [ ] `ProfilerPanel` and `PerformanceHUD` (F3) display live metrics and leak watermarks in Dear ImGui.
- [ ] Long-running multi-thousand frame soak benchmark runs cleanly with 0 memory leaks.
- [ ] `STATUS.md` and roadmap updated with Phase 12 sign-off.
