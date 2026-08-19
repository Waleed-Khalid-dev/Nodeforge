# NodeForge — Phase 7 Implementation Plan: ChanOp Pipeline (CPU/SIMD Audio, Math & Channels)

> **Document Version:** 1.0.0  
> **Status:** Approved for Implementation  
> **Target Phase:** Phase 7 (ChanOps)  
> **Author:** Antigravity `project-planner` Agent  
> **Date:** 2026-08-19  

---

## 1. Overview & Architectural Vision

Phase 7 introduces the **ChanOp (Channel Operator)** pipeline to NodeForge. ChanOps represent time-varying numerical data streams, multi-channel waveforms, control signals, modulation sources (LFOs, springs, filters), and audio buffers.

ChanOps are the primary animation and control backbone of NodeForge, directly powering:
1. Real-time procedural parameter modulation (e.g. driving `Tex.Noise` strength, `Tex.Transform` rotation, or shader parameters without writing code).
2. Live oscilloscope/waveform visualization and telemetry analysis.
3. Audio/waveform synthesis, audio file loading, and frequency analysis.
4. Bidirectional bridging between GPU textures and CPU channels (`ChanToTex` and `TexToChan`).

---

## 2. Key Architecture Decisions (From Design Interview)

1. **SIMD-Optimized Contiguous `ChannelBuffer`:**
   - Upgrade `ChannelBuffer` from `std::vector<std::vector<float>>` to a contiguous, 16/32-byte cache-aligned flat storage model with channel offset metadata.
   - Enables AVX2/NEON vectorization for math and signal processing, minimal allocation overhead, and zero-copy slicing.
2. **Dual-Mode Time Slicing & Evaluation:**
   - **Time-Sliced Mode:** Evaluates sample deltas $\Delta t$ per frame for live continuous streams (sensors, real-time audio, interactive controls) to maintain zero latency.
   - **Full-Range/Static Mode:** Evaluates fixed or timeline-wide sample buffers for curves, LFO visualizers, keyframes, and table lookups.
3. **Three-Tier Parameter Driving Architecture:**
   - **Tier 1 (Direct Binding):** `Parameter::SetBoundChannel(nodeId, channelName)` with zero-overhead C++ pointer/cache lookup during graph evaluation.
   - **Tier 2 (Python/Expression Access):** Native pybind11 syntax `op('lfo1')['chan1']` and `op('math1').sample(index)`.
   - **Tier 3 (UI Export):** Drag-and-drop channel binding from the Channel Viewer directly onto parameter widgets.
4. **Complete Operator Suite & Interop:**
   - 9 Foundational ChanOps: `Chan.Const`, `Chan.Time`, `Chan.LFO`, `Chan.Noise`, `Chan.Math`, `Chan.Filter`, `Chan.Merge`, `Chan.Select`, `Chan.Trail`.
   - 2 Interop Bridge Nodes: `ChanToTex` (`ChopToTop`) and `TexToChan` (`TopToChop`).
   - 1 Audio Input Node: `Chan.AudioFileIn` (using `miniaudio`).
5. **Interactive Oscilloscope Viewer:**
   - High-performance `ImDrawList` multi-channel vector oscilloscope in `ViewerPanel` with per-channel color coding, zero-cross grid, zoom/pan, legend toggle, and cursor value inspection.
   - Mini waveform preview directly on node canvas bodies.

---

## 3. Directory Layout & Planned Files

```text
NodeForge/
├── docs/
│   ├── adr/
│   │   └── ADR-0007-chanop-pipeline-and-audio.md      # [NEW] Architectural decision record
│   └── operator-spec/
│       ├── TimeChanOp.md                              # [NEW] Specification
│       ├── LFOChanOp.md                               # [NEW] Specification
│       ├── NoiseChanOp.md                             # [NEW] Specification
│       ├── FilterChanOp.md                            # [NEW] Specification
│       ├── MergeChanOp.md                             # [NEW] Specification
│       ├── SelectChanOp.md                            # [NEW] Specification
│       ├── TrailChanOp.md                             # [NEW] Specification
│       ├── AudioFileInChanOp.md                       # [NEW] Specification
│       ├── ChanToTexOp.md                             # [NEW] Specification
│       └── TexToChanOp.md                             # [NEW] Specification
├── src/
│   ├── core/
│   │   ├── ChannelBuffer.h                            # [NEW] SIMD-aligned contiguous multi-channel buffer
│   │   └── ChannelBuffer.cpp                          # [NEW] SIMD math helpers & slicing
│   ├── graph/
│   │   ├── PinValue.h                                 # [MODIFY] Integrate new ChannelBuffer
│   │   └── CookContext.h                              # [MODIFY] Add TimeSlice metadata (sampleStart, sampleCount, rate)
│   ├── param/
│   │   ├── Parameter.h                                # [MODIFY] Add Channel Binding metadata & cache
│   │   └── Parameter.cpp                              # [MODIFY] Resolve bound channels on evaluation
│   ├── operators/
│   │   └── chan/
│   │       ├── ChanOp.h                               # [NEW] Base class for ChanOps (time slicing, channel queries)
│   │       ├── ChanOp.cpp                             # [NEW] Base ChanOp implementation
│   │       ├── ConstantChanOp.h / .cpp                # [MODIFY] Update for new ChannelBuffer
│   │       ├── TimeChanOp.h / .cpp                    # [NEW] Timeline & clock channel generator
│   │       ├── LFOChanOp.h / .cpp                     # [NEW] Multi-waveform oscillator (sine, tri, saw, square, pulse)
│   │       ├── NoiseChanOp.h / .cpp                   # [NEW] 1D Perlin/Simplex procedural noise channel
│   │       ├── MathChanOp.h / .cpp                    # [MODIFY] SIMD math, range remapping & combine channels
│   │       ├── FilterChanOp.h / .cpp                  # [NEW] Lag, Spring, and smoothing filter
│   │       ├── MergeChanOp.h / .cpp                   # [NEW] Channel set merging & conflict resolution
│   │       ├── SelectChanOp.h / .cpp                  # [NEW] Channel pattern filtering & renaming
│   │       ├── TrailChanOp.h / .cpp                   # [NEW] Real-time sliding history ring buffer
│   │       ├── AudioFileInChanOp.h / .cpp             # [NEW] Miniaudio-powered audio file decoder
│   │       ├── ChanToTexOp.h / .cpp                   # [NEW] ChanBuffer -> GPU Texture2D (ChopToTop)
│   │       └── TexToChanOp.h / .cpp                   # [NEW] GPU Texture2D -> ChanBuffer (TopToChop)
│   ├── python/
│   │   └── NodeForgeModule.cpp                        # [MODIFY] Expose ChanOp & ChannelBuffer to Python
│   └── ui/
│       ├── panels/
│       │   ├── ViewerPanel.h / .cpp                   # [MODIFY] High-res ImDrawList multi-channel oscilloscope
│       │   └── ParameterPanel.cpp                     # [MODIFY] Channel bind UI indicator & drag-drop target
│       └── canvas/
│           └── NodeCanvas.cpp                         # [MODIFY] Mini waveform preview rendering
└── tests/
    ├── unit/
    │   ├── channel_buffer_test.cpp                    # [NEW] SIMD layout & buffer manipulation unit tests
    │   ├── chanop_pipeline_test.cpp                   # [NEW] Complete operator cooking & math tests
    │   └── chan_param_bind_test.cpp                   # [NEW] Direct channel-to-parameter binding tests
    └── benchmark/
        └── chanop_benchmark.cpp                       # [NEW] High-throughput SIMD benchmark (1M samples/sec)
```

---

## 4. Task Breakdown & Execution Plan

### Subphase 7.1: Foundation & Architecture (P0)
- **Task 7.1.1 — ADR-0007 & Operator Specifications**
  - **Agent:** `project-planner` / `backend-specialist` | **Skill:** `clean-code`
  - **Input:** Roadmap specs & interview decisions.
  - **Output:** `docs/adr/ADR-0007-chanop-pipeline-and-audio.md` and all 10 operator specs in `docs/operator-spec/`.
  - **Verify:** All docs present and referenced in `STATUS.md`.

- **Task 7.1.2 — SIMD Contiguous `ChannelBuffer` Data Structure**
  - **Agent:** `backend-specialist` | **Skill:** `clean-code`
  - **Input:** Flat aligned vector layout design with planar channel slices.
  - **Output:** `src/core/ChannelBuffer.h`, `ChannelBuffer.cpp`, integrated into `src/graph/PinValue.h`.
  - **Verify:** `channel_buffer_test.cpp` passes with 100% assertions on allocation, channel indexing, SIMD alignment, and serialization.

- **Task 7.1.3 — CookContext Time-Slicing Model & Base `ChanOp` Class**
  - **Agent:** `backend-specialist` | **Skill:** `clean-code`
  - **Input:** Frame time delta, sample rate, timeline scrub state.
  - **Output:** `src/graph/CookContext.h` extensions and `src/operators/chan/ChanOp.h / .cpp`.
  - **Verify:** Time-sliced sample range computation verified across 30, 60, and 120 FPS targets.

---

### Subphase 7.2: Core Generator & Signal Processing ChanOps (P1)
- **Task 7.2.1 — Generators (`ConstantChanOp`, `TimeChanOp`, `LFOChanOp`, `NoiseChanOp`)**
  - **Agent:** `backend-specialist` | **Skill:** `clean-code`
  - **Input:** Waveform generators, phase accumulators, timeline clock.
  - **Output:** `TimeChanOp`, `LFOChanOp` (Sine/Tri/Saw/Square/Perlin), `NoiseChanOp` (1D Perlin/Simplex), upgraded `ConstantChanOp`.
  - **Verify:** Oscillations and frequencies mathematically exact in unit tests.

- **Task 7.2.2 — Modifiers & Filters (`MathChanOp`, `FilterChanOp`, `MergeChanOp`, `SelectChanOp`)**
  - **Agent:** `backend-specialist` | **Skill:** `clean-code`
  - **Input:** SIMD arithmetic, lag/spring/damping equations, channel pattern matcher.
  - **Output:** Upgraded `MathChanOp` (combine channels, arithmetic, remapping), `FilterChanOp` (Lag/Spring/Damp), `MergeChanOp`, `SelectChanOp`.
  - **Verify:** Impulse response, lag convergence, and channel merging tested in `chanop_pipeline_test.cpp`.

- **Task 7.2.3 — History Buffer (`TrailChanOp`)**
  - **Agent:** `backend-specialist` | **Skill:** `clean-code`
  - **Input:** Ring buffer over sliding time window $N$ frames/seconds.
  - **Output:** `TrailChanOp.h / .cpp`.
  - **Verify:** Dynamic continuous ring buffer append and sample truncation verified.

---

### Subphase 7.3: Interop, Audio & Parameter Binding (P1)
- **Task 7.3.1 — Parameter Channel Binding System (`Chan -> Param`)**
  - **Agent:** `backend-specialist` | **Skill:** `clean-code`
  - **Input:** Direct parameter channel reference metadata and cache evaluation.
  - **Output:** `src/param/Parameter.h / .cpp` bound channel evaluation, Python `op('lfo1')['chan1']` operator overload.
  - **Verify:** `chan_param_bind_test.cpp` proves parameters dynamically follow LFO without Python overhead.

- **Task 7.3.2 — Texture/Channel Interop Bridges (`ChanToTexOp` & `TexToChanOp`)**
  - **Agent:** `backend-specialist` | **Skill:** `clean-code`
  - **Input:** GPU texture pixel readback to channels, and channel buffer rasterization to 1D/2D Vulkan textures.
  - **Output:** `ChanToTexOp` (`ChopToTop`) and `TexToChanOp` (`TopToChop`).
  - **Verify:** Round-trip texture-to-channel-to-texture data integrity verified.

- **Task 7.3.3 — Audio File Decoding (`AudioFileInChanOp`)**
  - **Agent:** `backend-specialist` | **Skill:** `clean-code`
  - **Input:** `miniaudio` decoder (header-only / vcpkg).
  - **Output:** `AudioFileInChanOp.h / .cpp` decoding WAV/MP3/FLAC into multi-channel audio buffers.
  - **Verify:** Audio file decode and playback streaming test in unit test suite.

---

### Subphase 7.4: IDE UI & Visualization (P2)
- **Task 7.4.1 — Multi-Channel Oscilloscope Visualizer in `ViewerPanel`**
  - **Agent:** `frontend-specialist` | **Skill:** `clean-code`
  - **Input:** Vector rendering of multi-channel data via `ImDrawList`.
  - **Output:** Oscilloscope display, color legend, grid, zoom/fit controls, tooltip readout in `ViewerPanel.cpp`.
  - **Verify:** Visual rendering of 1000-sample multi-channel buffer at 60 FPS without frame drops.

- **Task 7.4.2 — Mini Waveform Previews & Drag-Drop Parameter Binding**
  - **Agent:** `frontend-specialist` | **Skill:** `clean-code`
  - **Input:** Node canvas draw hooks and ImGui drag-and-drop payloads.
  - **Output:** Mini scope inside ChanOp node headers, channel drag to parameter field binding in `ParameterPanel.cpp`.
  - **Verify:** Dragging a channel name onto a parameter slider establishes real-time binding.

---

### Subphase 7.5: Testing, Benchmarks & Verification (Phase X)
- **Task 7.5.1 — Comprehensive Unit & Integration Test Suite**
  - **Agent:** `backend-specialist` | **Skill:** `testing-patterns`
  - **Output:** `tests/unit/channel_buffer_test.cpp`, `chanop_pipeline_test.cpp`, `chan_param_bind_test.cpp`.
  - **Verify:** 100% test pass rate across all new ChanOps and binding workflows.

- **Task 7.5.2 — High-Throughput Performance Benchmark**
  - **Agent:** `backend-specialist` | **Skill:** `performance-profiling`
  - **Output:** `tests/benchmark/chanop_benchmark.cpp`.
  - **Verify:** >1,000,000 samples/second throughput on SIMD math and filtering with 0 memory leaks across 10,000 frames.

---

## 5. Definition of Done (Phase 7 DoD)

- [ ] `ChannelBuffer` SIMD-aligned flat memory model implemented and fully tested.
- [ ] All 10 new operator specifications documented in `docs/operator-spec/`.
- [ ] ADR-0007 documented in `docs/adr/`.
- [ ] All 9 core ChanOps (`Const`, `Time`, `LFO`, `Noise`, `Math`, `Filter`, `Merge`, `Select`, `Trail`) implemented and registered.
- [ ] `ChanToTexOp` and `TexToChanOp` bridge nodes functional.
- [ ] `AudioFileInChanOp` loads audio files into channels via `miniaudio`.
- [ ] Direct Parameter Binding (`Chan -> Param`) drives GPU TexOps (e.g. `Chan.LFO` modulating `Tex.Noise` strength) smoothly at 60 FPS.
- [ ] Interactive multi-channel oscilloscope in `ViewerPanel` and mini previews on canvas nodes.
- [ ] All unit and benchmark tests pass cleanly with zero memory leaks.
- [ ] `STATUS.md` updated with Phase 7 signed off.
