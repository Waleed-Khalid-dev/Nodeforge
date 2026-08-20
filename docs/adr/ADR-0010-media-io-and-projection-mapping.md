# ADR-0010: Media I/O, Texture Sharing & Projection Mapping Architecture

## Status
Accepted

## Context
NodeForge is designed as a premier real-time visual development platform, serving flagship commercial workflows for Neo Realms including large-scale multi-projector building façade activations, interactive holograms, and walkable floor plans. 

To power these workflows, NodeForge requires:
1. High-throughput video file decoding (`MovieFileInTexOp`) with seamless looping, scrub indexing, reverse playback, and audio synchronization.
2. Real-time live camera / HDMI capture (`VideoDeviceInTexOp`).
3. Zero-latency inter-application GPU texture sharing on Windows via Spout2 (`SpoutInTexOp`, `SpoutOutTexOp`).
4. Network video streaming across LAN via NDI (`NDIInTexOp`, `NDIOutTexOp`).
5. Multi-projector output window routing (`ProjectorOutTexOp`).
6. Sub-millisecond 2D Bezier grid warping, gamma-corrected softedge blending, black-level pedestal lift, and on-site interactive calibration overlay (`WarpBlendTexOp`).

## Decision

### 1. Multi-Threaded Asynchronous Video Engine (`VideoDecoder`)
- Background worker thread with lockless ring buffer (`std::queue` with mutex / atomic index) for decoded frames.
- Frame cache maintaining decoded RGBA frames ready for Vulkan staging buffer DMA transfer.
- Support for playback speed (0.1x to 10.0x), play/pause, looping modes (Loop, Once, Ping-Pong), frame-accurate time scrub, and aspect ratio matching.

### 2. Multi-Display Output Window Manager (`DisplayManager`)
- Enumerates all physical monitors attached to the host machine using Win32 / GLFW APIs.
- Manages independent borderless fullscreen display windows per target projector.
- Synchronizes swapchain presentation across outputs with V-Sync.
- Minimal on-site fullscreen calibration UI (F11 toggle) for direct on-screen warp point manipulation without opening the full IDE.

### 3. GPU Warp & Edge-Blend Pipeline (`WarpBlendPass`)
- Configurable $N \times M$ control grid (2x2 four-corner pin up to 16x16 bicubic Bezier surface).
- Gamma-corrected S-curve softedge blending (luminance-compensated smoothstep curve).
- Black-level pedestal lift to balance projector black floor overlap.
- Integrated test pattern generator: 8-color SMPTE bars, crosshair alignment grid, concentric alignment circles, and checkerboards.

### 4. GPU Interop & Capture (`Spout2` & `MediaFoundation`)
- Windows DXGI / Vulkan shared memory handles (`VK_KHR_external_memory_win32`) for zero-copy 60+ FPS texture exchange.
- Media Foundation asynchronous capture engine for webcams and USB capture devices.
- NDI network video streaming pipeline.

## Consequences
- **Positive:** Delivers Neo Realms' primary commercial workflow with zero external media server dependencies.
- **Positive:** Multi-threaded decoding and GPU rasterized warping ensure rock-solid 60 FPS output across multiple 1080p / 4K displays with zero memory leaks.
- **Verification:** Unit tests and soak benchmarks verifying 8+ hour continuous playback, multi-output swapchain presentation, warp serialization, and zero GPU memory leaks.
