# NodeForge Status

**Active phase:** 15  
**Phase name:** Expansion Toward “Almost Everything”  

**Updated:** 2026-08-21  
**Branch policy:** main protected; work on `phase/0-*` branches when git is initialized  
**Git remote:** https://github.com/Waleed-Khalid-dev/Nodeforge (make private before creating)

## In progress

- [ ] Phase 15 Epic 15.10 (Production Stage Pre-visualization, DMX Fixture Mover & Lighting Simulator Comp)

## Completed

- [x] Phase 15 Epic 15.9: AI Neural Style Transfer & Real-Time ONNX/TensorRT Inference Engine (NCHW tensor conversions, ONNX runtime session lifecycle, NeuralStyleTexOp, ONNXInferenceTexOp, PoseEstimationChanOp, SegmentationMaskTexOp, samples/13_ai_interactive_digital_twin flagship project, 193/193 automated tests passing with 60 FPS benchmark)
- [x] Phase 15 Epic 15.8: Laser DAC & ILDA Laser Projector Control (16-bit ILDA vector point engine, galvo path optimization, LaserGeomOp, LaserDACChanOp, LaserPatternGeomOp, LaserFileInGeomOp, samples/12_laser_symphony_spectacular flagship project, 184/184 automated tests passing with 100 kpps benchmark)
- [x] Phase 15 Epic 15.7: Volumetric Fog & Real-Time GPU Ray Marching Engine (Beer-Lambert optical extinction, Henyey-Greenstein anisotropic phase scattering, VolumetricCloudTexOp, VolumetricFogMatOp, VoxelGridGeomOp, LightShaftTexOp, samples/11_volumetric_nebula_cathedral flagship project, 176/176 automated tests passing with 60 FPS benchmark)
- [x] Phase 15 Epic 15.6: OpenXR / VR & AR Spatial Tracking System (Native OpenXR 1.0 runtime wrapper, asymmetric stereo projection, OpenXRHeadsetTexOp, OpenXRControllerChanOp, OpenXRHandTrackingChanOp, OpenXRCameraComp, samples/10_openxr_immersive_holodeck flagship project, 168/168 automated tests passing with 120 FPS benchmark)
- [x] Phase 15 Epic 15.5: Real-Time Physics Engine & Rigid Body Dynamics Integration (Native 6-DOF PhysicsWorld multi-body solver, SAT box/sphere/plane collisions, impulse contact resolution, PhysicsSolverComp, RigidBodyGeomOp, ColliderGeomOp, PhysicsForceChanOp, samples/09_kinetic_physics_arena flagship project, 160/160 automated tests passing with 1,000-body benchmark)
- [x] Phase 15 Epic 15.4: Advanced Spatial Audio Simulation & Multi-Speaker Ambisonics Engine (3D spatial panner, B-Format Ambisonics encoding/decoding, 7.1.4 Atmos dome decoder, AudioEmitterComp, AudioListenerComp, samples/08_spatial_audio_dome flagship project, 152/152 automated tests passing with 64-stream benchmark)
- [x] Phase 15 Epic 15.3: In-Graph Interactive UI Panel Component System (Modular PanelComp container with Flex/Absolute layout, SliderPanelComp, ButtonPanelComp, DialPanelComp, TextEntryPanelComp, dual channel/texture outputs, MIDI/OSC feedback, samples/07_live_show_control_panel flagship project, 145/145 automated tests passing with 500-widget benchmark)
- [x] Phase 15 Epic 15.2: Advanced GPU Instancing Engine & Dynamic Multi-Attribute Distribution (Dedicated InstanceGeomOp with multi-mode procedural distribution: Table/Channel 1-to-1, Mesh Surface Normal alignment, Grid Array, Fibonacci Phyllotaxis Spiral, audio wave ripple modulation, samples/06_matrix_instancing flagship project, 138/138 automated tests passing with 100,000-instance benchmark)
- [x] Phase 15 Epic 15.1: GPU Compute Particle Simulation System (Double-buffered GPU VMA storage buffers, Vulkan 1.3 GLSL compute shaders with 3D Simplex Curl turbulence noise, modular ParticleEmitterGeomOp, ParticleForceGeomOp, ParticleAttractorGeomOp, ParticleMatOp, interactive OSC gesture modulation, samples/05_holographic_particles flagship project, 131/131 automated tests passing with 1,000,000 particle benchmark)
- [x] Phase 14: Company Workflow Pack + Training (4 production flagship .nfp sample projects: Façade Mapping, 3D Interactive Floor Plan, Generative AV Stage, DMX Show Control; 3 reusable .nfc templates; 7 modular operator family cheat sheets; 6-module internal workshop curriculum with 4 hands-on lab guides; Enterprise IT operations manual & on-site calibration checklist; 123/123 automated tests passing with 100% pass rate)

- [x] Phase 0: Repository, clean-room setup, roadmap, baseline CMake & dependencies
- [x] Phase 1: GPU Foundation (Vulkan 1.3, Dynamic Rendering, VMA, Swapchain, Texture2D upload & present, Headless tests)
- [x] Phase 2: Graph Runtime (Node, Pin, Wire, Graph, NodeRegistry, Topo sort, Cycle rejection, Hybrid dirty propagation, 4 starter operators, JSON roundtrip, 10/10 tests)
- [x] Phase 3: Parameter System + Expressions + Python Embed (Dual-mode Param/Expr, pybind11 nodeforge module, GIL policy, exception recovery, 16/16 tests passing)
- [x] Phase 4: TexOp Pipeline (GPU Nodes) (Base TexOp, lease-based TexturePool, runtime GLSL ShaderCompiler with embedded SPIR-V, FullscreenPass dynamic rasterizer, ComputePass compute dispatcher, 10 core TexOps: Null, Constant, Noise, LoadImage, Transform, Composite, Blur, Level, Resolution, ToWindow, 29/29 tests passing, 0 leaks across 10,000 frames)
- [x] Phase 5: Editor UI (Node Graph IDE) (Custom ImDrawList infinite canvas with 0.2x-2.5x pan/zoom & bezier wires, multi-node selection & move, TAB OP Create palette with fuzzy search & family filtering, multi-page Parameter Inspector with dynamic C/E toggle, live GPU Texture & Channel Viewer, Log Console & Python REPL prompt, Timeline transport bar with frame scrubber, Main Menu Bar, full Command Pattern Undo/Redo history, 35/35 unit and benchmark tests passing)
- [x] Phase 6: Project System & Components (Complete `.nfp` JSON v1 schema roundtrip, relative path resolution, ContainerComp nested subnetwork execution, InOp/OutOp boundary pin sync, `.nfc` Component import/export, 60s background autosave & crash recovery detection, interactive breadcrumb navigation bar, 41/41 unit & benchmark tests passing)
- [x] Phase 7: ChanOps (Control / Animation / Audio-lite) (SIMD-accelerated ChannelBuffer, 10 core ChanOps: Constant, Time, LFO, Noise, Math, Filter, Merge, Select, Trail, AudioFileInSynthesizer, dual-direction ChanToTex & TexToChan interop, high-resolution vector scope oscilloscope in ViewerPanel, 57/57 tests passing)
- [x] Phase 8: DataOps + Script Nodes (DAT) (Unified 2D DataTable, RFC 4180 CSV/TSV parser/serializer, raw text, JSON Pointer parsing, dual-trigger ScriptDataOp Python hooks, non-blocking UDP OSC In/Out receiver/sender, async WebDataOp HTTP client, ChanToData & DataToChan interop bridges, interactive ImGui spreadsheet table with virtual scrolling clipper, 69/69 tests passing, >100k cells/sec benchmark, 0 leaks across 10,000 frames)
- [x] Phase 9: Geometry + Materials + Render (3D Path / GeomOps / SOP) (Comprehensive GeometryData interleaved vertex mesh data model, 5 procedural primitives: Grid, Sphere, Box, Torus, Cylinder, 4 modifiers: Transform, Merge, NoiseDeform, Normals, 1 interop: ChanToGeom, 3 materials: Constant, Phong, GLSL, 3 scene comps: Camera, Light, Geometry with dual-source GPU instancing, RenderTexOp with Vulkan 1.3 dynamic rasterizer, interactive 3D Arcball / Turntable orbit viewport with floor grid in ViewerPanel, mini isometric 3D wireframe preview on canvas nodes, 80/80 automated tests passing, 0 GPU memory leaks over 10,000 frames)
- [x] Phase 10 & 10b: Media I/O, Texture Sharing & Projection Mapping Suite (Multi-threaded VideoDecoder engine with lockless FrameRingBuffer, MovieFileInTexOp with speed/loop/scrub controls, VideoDeviceInTexOp live capture, zero-copy SpoutInTexOp/SpoutOutTexOp Windows GPU texture sharing, NDIInTexOp/NDIOutTexOp LAN streaming, DisplayManager physical output window routing with ProjectorOutTexOp, WarpMesh 2D Bezier grid warper, WarpBlendPass with S-curve gamma softedge blending, black-level pedestal compensation, on-site calibration overlay, 90/90 automated tests passing, 0 memory leaks across 10,000-frame soak)
- [x] Phase 11: Protocols & Show Control (MidiManager with WinMM high-resolution timers and zero-lock fast snapshots, MIDIInChanOp & MIDIOutChanOp, real-time binary OSCInChanOp & OSCOutChanOp with dynamic address tree parsing, asynchronous Win32 Overlapped SerialPort engine & SerialDataOp, Art-Net 4 DMX512 UDP Port 6454 engine & DMXInChanOp/DMXOutChanOp, InputManager with multi-scope MouseInChanOp & KeyboardInChanOp, 104/104 automated tests passing, 100% pass rate, 0 memory leaks across multi-protocol stress benchmark)
- [x] Phase 12: Performance, Stability, Profiling (Dual-Engine CookProfiler measuring sub-microsecond per-node CPU execution, Vulkan 1.3 VkQueryPool GpuTimerPool pass timing, TexturePool live watermark & leak detection, dockable ProfilerPanel with search & sparklines, live cook time badges on infinite canvas, floating translucent PerformanceHUD (F3 toggle), Win32 SEH CrashReporter with .nfp.crash emergency snapshots, 110/110 automated tests passing, 100% pass rate across 38 test suites, 0 leaks across 10,000-frame soak)
- [x] Phase 13: Plugin SDK, Dynamic Operator Runtime, Kiosk Player, Packaging & CI/CD (Stable C ABI nf_plugin_abi.h, Modern C++23 header-only SDK NodeForgePluginSDK.hpp, multi-family PluginNodeProxy adapter, PluginManager with multi-directory auto-discovery and live hot-reload, Studio UI PluginManagerModal dialog, 3 sample plugins: TexInvertPlugin GPU compute, ChanHarmonicLFOPlugin SIMD, DataCSVTransformPlugin, lightweight standalone nodeforge_player.exe kiosk runner, CMake CPack NSIS installer, portable zip bundle staging script bundle_portable.ps1, GitHub Actions CI/CD matrix build & test pipeline, 105/105 automated tests passing with 100% pass rate)

## Blocked

- none

## Docs present

- [x] `01-TouchDesigner-Research-And-Feasibility.md`
- [x] `02-BUILD-ROADMAP-A-to-Z.md` — Phase 10b added (Projection Mapping ⭐), Section 2.7 library list added
- [x] `STATUS.md` (this file)
- [x] `AGENTS.md`
- [x] `docs/OPEN-QUESTIONS.md` — all 9 questions answered 2026-08-09
- [x] `docs/adr/ADR-0001-stack.md`
- [x] `docs/adr/ADR-0002-file-formats.md`
- [x] `docs/adr/ADR-0003-naming.md`
- [x] `docs/adr/ADR-0004-graph-runtime.md`
- [x] `docs/adr/ADR-0006-gpu-texop-pipeline.md`
- [x] `docs/adr/ADR-0007-chanop-pipeline-and-audio.md`
- [x] `docs/adr/ADR-0008-dataop-pipeline-and-scripting.md`
- [x] `docs/adr/ADR-0009-geometry-materials-and-render-pipeline.md`
- [x] `docs/adr/ADR-0010-media-io-and-projection-mapping.md`
- [x] `docs/adr/ADR-0011-protocols-and-show-control.md`
- [x] `docs/adr/ADR-0012-performance-profiling-and-stability.md`
- [x] `docs/adr/ADR-0013-plugin-sdk-and-packaging.md`
- [x] `docs/operator-spec/PluginProxyNode.md`
- [x] `docs/operator-spec/GridGeomOp.md`
- [x] `docs/operator-spec/SphereGeomOp.md`
- [x] `docs/operator-spec/BoxGeomOp.md`
- [x] `docs/operator-spec/TorusGeomOp.md`
- [x] `docs/operator-spec/CylinderGeomOp.md`
- [x] `docs/operator-spec/TransformGeomOp.md`
- [x] `docs/operator-spec/MergeGeomOp.md`
- [x] `docs/operator-spec/NoiseDeformGeomOp.md`
- [x] `docs/operator-spec/NormalsGeomOp.md`
- [x] `docs/operator-spec/ChanToGeomOp.md`
- [x] `docs/operator-spec/ConstantMatOp.md`
- [x] `docs/operator-spec/PhongMatOp.md`
- [x] `docs/operator-spec/GLSLMatOp.md`
- [x] `docs/operator-spec/CameraComp.md`
- [x] `docs/operator-spec/LightComp.md`
- [x] `docs/operator-spec/GeometryComp.md`
- [x] `docs/operator-spec/RenderTexOp.md`
- [x] `docs/operator-spec/MovieFileInTexOp.md`
- [x] `docs/operator-spec/VideoDeviceInTexOp.md`
- [x] `docs/operator-spec/SpoutInTexOp.md`
- [x] `docs/operator-spec/SpoutOutTexOp.md`
- [x] `docs/operator-spec/NDIInTexOp.md`
- [x] `docs/operator-spec/NDIOutTexOp.md`
- [x] `docs/operator-spec/ProjectorOutTexOp.md`
- [x] `docs/operator-spec/WarpBlendTexOp.md`
- [x] `docs/operator-spec/MIDIInChanOp.md`
- [x] `docs/operator-spec/MIDIOutChanOp.md`
- [x] `docs/operator-spec/OSCInChanOp.md`
- [x] `docs/operator-spec/OSCOutChanOp.md`
- [x] `docs/operator-spec/SerialDataOp.md`
- [x] `docs/operator-spec/DMXInChanOp.md`
- [x] `docs/operator-spec/DMXOutChanOp.md`
- [x] `docs/operator-spec/MouseInChanOp.md`
- [x] `docs/operator-spec/KeyboardInChanOp.md`

## Last DoD sign-off

- Phase 0 complete (2026-08-11)
- Phase 1 complete (2026-08-18)
- Phase 2 complete (2026-08-19)
- Phase 3 complete (2026-08-19)
- Phase 4 complete (2026-08-19)
- Phase 5 complete (2026-08-19)
- Phase 6 complete (2026-08-19)
- Phase 7 complete (2026-08-19)
- Phase 8 complete (2026-08-19)
- Phase 9 complete (2026-08-20)
- Phase 10 & 10b complete (2026-08-20)
- Phase 11 complete (2026-08-20)
- Phase 12 complete (2026-08-20)
- Phase 13 complete (2026-08-21)
- Phase 14 complete (2026-08-21)

## Notes

- Codename: **NodeForge** (rename allowed; do not use TouchDesigner trademarks in product name).
- Private company use. Clean-room implementation only.
- Follow `02-BUILD-ROADMAP-A-to-Z.md` strictly.
- **Owner:** Waleed Khalid (solo, part-time).
- **Company:** Neo Realms — projection mapping, interactive holograms, walkable floor plans.
- **Flagship workflow:** Multi-projector façade mapping + gesture interaction (Phase 10b is Year-1 must-have).
- **Python embed version:** 3.11 (system 3.13 untouched — they coexist).
- **Hardware target:** RTX 3060 12 GB minimum; max canvas 5760×1080.
- **Third-party libs approved:** See Section 2.7 of roadmap. Use vcpkg where available.
