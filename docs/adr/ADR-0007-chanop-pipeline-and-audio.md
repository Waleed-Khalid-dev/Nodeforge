# ADR-0007: ChanOp Pipeline Architecture, SIMD Contiguous ChannelBuffer & Dual-Mode Time-Slicing

## Status
Accepted (Phase 7)

## Context
In real-time procedural environments like NodeForge, numerical control signals, modulation sources (LFOs, springs, filters), timeline clocks, and multi-channel audio data are represented by **ChanOps (Channel Operators)**.

Previously in Phase 2, a minimal `ChannelBuffer` was introduced with `std::vector<std::vector<float>>` storage to test basic DAG propagation. However, as NodeForge scales to Phase 7:
1. Multi-channel audio and dense animation curves require cache-contiguous memory to support vectorization (AVX2/NEON/SSE), zero-copy channel views, and high-throughput signal processing (>1,000,000 samples/sec).
2. Live animation and sensor integration require deterministic **Time-Slicing** (computing sample deltas $\Delta t$ per frame without latency accumulation) alongside **Full-Range** static buffer evaluation (for curves, LFO visualizers, and keyframe tables).
3. Parameter animation requires a zero-overhead parameter-to-channel binding mechanism where node parameters can subscribe directly to specific channels without incurring Python interpreter overhead on each frame.
4. Bidirectional bridging between GPU textures and CPU channels (`ChanToTexOp` and `TexToChanOp`) is necessary for audio-reactive visuals and compute-driven modulation.

## Decision

### 1. Contiguous SIMD-Aligned `ChannelBuffer` Layout
`ChannelBuffer` is restructured to hold planar float sample channels in a single contiguous buffer:
- Channel samples are stored consecutively with aligned channel strides ($32$-byte alignment).
- Metadata includes: channel names (`std::vector<std::string>`), sample rate ($Hz$), start sample index / timeline offset, and sample count.
- Provides SIMD-friendly vector arithmetic functions (`Add`, `Multiply`, `Remap`, `Clamp`, `Mix`, `Min`, `Max`).

### 2. Dual-Mode Time-Slicing Execution Model
The `CookContext` is extended to carry `TimeSlice` information:
- `timeSlice.isTimeSliced`: True when evaluating continuous real-time streams.
- `timeSlice.startSample`: Global timeline start sample for the frame.
- `timeSlice.sampleCount`: Number of samples corresponding to frame delta $\Delta t$ ($N = \text{rate} \times \Delta t$).
- `timeSlice.sampleRate`: Base clock rate (default 60.0 Hz).
- ChanOps can evaluate in time-slice mode (outputting $N$ new samples per frame) or full-range mode (outputting full curve buffers). `Chan.Trail` provides a ring buffer to bridge time-sliced streams into historical scope windows.

### 3. Three-Tier Parameter Driving Architecture
Parameters are enriched with channel binding capabilities:
- **Tier 1 (Native Direct Binding):** `Parameter::SetBoundChannel(nodeId, channelName)` caches the target channel and reads the latest sample during graph evaluation with zero scripting overhead.
- **Tier 2 (Python / Expressions):** `op('lfo1')['chan1']` index operator overload and `.eval()` methods exposed via pybind11.
- **Tier 3 (UI Export):** Drag-and-drop channel binding from the Channel Viewer directly onto parameter slider/input widgets.

### 4. GPU/CPU Interop Bridges
- `ChanToTexOp` (`ChopToTop`): Encodes multi-channel numeric data into 1D/2D Vulkan RGBA32F or RGBA8 textures for vertex displacement, audio-reactive shaders, and color LUTs.
- `TexToChanOp` (`TopToChop`): Reads back GPU render target lines or pixels into CPU channel buffers for luminance tracking, audio meters, or optical flow analysis.

## Consequences
- High-performance, zero-allocation signal processing across all ChanOps.
- Clean integration with existing Graph/DAG runtime, Python embed, and Dear ImGui editor UI.
- All operator specifications maintained in `docs/operator-spec/`.
