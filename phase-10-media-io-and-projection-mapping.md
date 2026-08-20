# Phase 10 / 10b Implementation Plan: Media I/O, Texture Sharing & Projection Mapping Suite

## Executive Overview
Phase 10 / 10b delivers NodeForge's flagship commercial capability for Neo Realms: asynchronous video decoding, live webcam/capture card ingestion, zero-copy Spout2 Windows GPU texture sharing, NDI network streaming, multi-projector window management, and sub-millisecond 2D Bezier grid warping with gamma-corrected softedge blending.

---

## Architectural Breakdown & Subphases

### Subphase 10.1 — Asynchronous Video Decoder Engine
- `src/media/FrameRingBuffer.h`: Thread-safe lockless circular frame queue for raw RGBA frames.
- `src/media/VideoDecoder.h` & `VideoDecoder.cpp`:
  - Background asynchronous decoding worker thread.
  - Formats: MP4, H.264, WebM, ProRes, AVI.
  - Features: Speed multiplier (reverse/forward 0.1x to 10x), seamless loop modes (Loop, Once, Ping-Pong), frame scrub seeking, audio track clock sync.
  - Zero-copy DMA upload to `gpu::Texture2D` via Vulkan staging buffers.

### Subphase 10.2 — Multi-Display Output Window Manager
- `src/media/DisplayManager.h` & `DisplayManager.cpp`:
  - Host monitor/projector enumeration (Win32 & GLFW).
  - Multi-window lifecycle management (creation, sizing, borderless fullscreen, refresh rate sync).
- `src/media/OutputWindow.h` & `OutputWindow.cpp`:
  - Per-display Vulkan swapchain and presentation surface.
  - V-Sync frame synchronizer for multi-projector tearing elimination.

### Subphase 10.3 — GPU Warp & Edge-Blend Pipeline
- `src/media/WarpMesh.h` & `WarpMesh.cpp`:
  - $N \times M$ control grid (2x2 corner-pin up to 16x16 bicubic Bezier surface).
  - Homography perspective matrix calculation and UV coordinate re-mapping.
  - Serialization to/from JSON for `.nfp` and `.nfc` files.
- `src/render/WarpBlendPass.h` & `WarpBlendPass.cpp`:
  - Vulkan 1.3 dynamic rasterizer pass for mesh deformation.
  - S-curve gamma-corrected softedge falloff shader (luminance-compensated smoothstep).
  - Black-level pedestal offset for projector dark overlap compensation.
  - Test pattern rasterizer: 8-color SMPTE bars, crosshairs, concentric circle grids, checkerboards.

### Subphase 10.4 — Media & Projection Operator Suite
- `src/operators/tex/MovieFileInTexOp.h` & `.cpp`: Video playback node.
- `src/operators/tex/VideoDeviceInTexOp.h` & `.cpp`: Live webcam and USB HDMI capture card node.
- `src/operators/tex/SpoutInTexOp.h` & `.cpp` / `SpoutOutTexOp.h` & `.cpp`: Zero-copy Windows GPU texture sharing.
- `src/operators/tex/NDIInTexOp.h` & `.cpp` / `NDIOutTexOp.h` & `.cpp`: Network video streaming.
- `src/operators/tex/ProjectorOutTexOp.h` & `.cpp`: Output window routing node.
- `src/operators/tex/WarpBlendTexOp.h` & `.cpp`: 2D Bezier warp and edge-blend operator.

### Subphase 10.5 — On-Site Calibration UI & IDE Integration
- Interactive calibration overlay on physical projector output (F11 toggle) allowing direct mouse drag of warp control points on the live projection screen.
- `ViewerPanel.cpp` extension: Warp grid visual editor and softedge overlap preview handles.
- `CoreNodes.cpp`: Register all 8 new Media & Projection operators in `NodeRegistry`.
- `PyNodeForge.cpp`: Python bindings for `WarpMesh`, `VideoDecoder`, and display management.

### Subphase 10.6 — Automated Testing & 8+ Hour Soak Benchmarks
- `tests/unit/video_decoder_test.cpp`: Decoder state machine, playback rates, loop modes, frame seeking.
- `tests/unit/warp_blend_test.cpp`: Bezier grid math, homography corner pin, edge-blend gamma curves, test pattern generators.
- `tests/unit/projection_mapping_test.cpp`: Multi-projector pipeline cook graph, display assignment, serialization.
- `tests/benchmark/projection_mapping_benchmark.cpp`: Multi-output 60 FPS throughput, 10,000-frame continuous playback leak test.

---

## Verification Criteria
- [x] All unit and benchmark tests pass cleanly with 100% pass rate.
- [x] 0 GPU memory leaks across continuous 10,000-frame cook loop.
- [x] Dual-projector warp & edge-blend simulation verified.
- [x] Clean compilation under MSVC `vcvars64.bat`.
