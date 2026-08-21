# Memory Index

## User Preferences
- [user] Professional, direct, and concise communication style with structured implementation plans -> user-preferences.md
- [user] Requires VPN (WARP/1.1.1.1) for large vcpkg package downloads -> tech-decisions.md

## Project & Architecture
- [project] NodeForge Core Stack: C++23, Vulkan 1.3, Dear ImGui, CPython 3 embed, vcpkg -> architecture.md
- [project] Workspace directory has brackets `D:\[Project]\Touch Designer`; use `-LiteralPath` in PowerShell and `git -C` with backtick escapes -> project-conventions.md
- [project] Phase 0 to Phase 14 + Phase 15 Epics 15.1, 15.2, 15.3, 15.4, 15.5, 15.6, 15.7, 15.8 & 15.9 (GPU presentation, DAG Runtime, Python 3 Embed, TexOp Pipeline, Studio Editor UI, Project System & Components, ChanOp SIMD Audio/Channels, DataOps & Scripting, Geometry Engine & 3D Vulkan Render Pipeline, Media I/O & Projection Mapping Suite, Protocols & Show Control, Performance Profiler & Crash Diagnostics, Plugin SDK & Packaging, Company Workflow Pack & Training Suite, GPU Compute Particles, GPU Instancing Engine, In-Graph UI Panels, Spatial Audio & Ambisonics Engine, Real-Time Physics & Rigid Body Dynamics, OpenXR VR & AR Spatial Tracking, Volumetric Fog & Ray Marching Engine, Laser DAC & ILDA Control, AI Neural Style & Inference, 193/193 tests) COMPLETE -> STATUS.md
- [project] Active Phase: Phase 15 (Expansion Toward “Almost Everything” — Stage Pre-vis, DMX Lighting Simulator) -> STATUS.md
- [project] Hybrid pull-on-demand with push dirty invalidation, PinValue variant, and Kahn's topo-sort -> docs/adr/ADR-0004-graph-runtime.md
- [project] Parameter System & Python Embedding Architecture (dual-mode Constant/Expression, pybind11, GIL management) -> docs/adr/ADR-0005-parameter-system-and-python.md
- [project] GPU TexOp Pipeline & Shader Architecture (FullscreenPass, ComputePass, dynamic raster/compute) -> docs/adr/ADR-0006-gpu-texop-pipeline.md
- [project] ChanOp Pipeline & Audio Architecture (SIMD contiguous ChannelBuffer, dual time-slicing, 3-tier parameter binding) -> docs/adr/ADR-0007-chanop-pipeline-and-audio.md
- [project] DataOp Pipeline & Scripting Architecture (DataTable 2D grid, Python onCook/onPulse, OSC In/Out, Web async HTTP) -> docs/adr/ADR-0008-dataop-pipeline-and-scripting.md
- [project] 3D Geometry Engine, Materials & Vulkan Dynamic Render Pipeline (GeometryData interleaved vertex mesh, 10 GeomOps, 3 MatOps, Camera/Light/Geometry Comps, Scene3DPass, RenderTexOp) -> docs/adr/ADR-0009-geometry-materials-and-render-pipeline.md
- [project] Media I/O, Texture Sharing & Projection Mapping Architecture (VideoDecoder, FrameRingBuffer, Spout2/NDI zero-copy interop, DisplayManager, WarpMesh 2D Bezier grid, WarpBlendPass) -> docs/adr/ADR-0010-media-io-and-projection-mapping.md
- [project] Protocols & Show Control Architecture (MidiManager, SerialPort Win32 Overlapped, ArtNetEngine UDP 6454 DMX512, InputManager multi-scope Mouse/Keyboard) -> docs/adr/ADR-0011-protocols-and-show-control.md
- [project] Performance Profiling, Stability & Crash Recovery Architecture (CookProfiler, GpuTimerPool VkQueryPool, TexturePool leak detector, ProfilerPanel, PerformanceHUD, Win32 SEH CrashReporter) -> docs/adr/ADR-0012-performance-profiling-and-stability.md
- [project] Plugin SDK, Dynamic Operator Runtime, Kiosk Player, Packaging & CI/CD Architecture (C ABI, C++23 SDK, PluginNodeProxy, PluginManager hot-reload, nodeforge_player.exe, CPack NSIS/ZIP) -> docs/adr/ADR-0013-plugin-sdk-and-packaging.md
- [project] GPU Compute Particle Simulation System Architecture (double-buffered VkBuffer, 3D curl turbulence, point attractors, point-sprite/billboard ParticleMatOp) -> docs/adr/ADR-0014-gpu-compute-particles.md
- [project] Advanced GPU Instancing Engine Architecture (InstanceGeomOp, secondary vertex buffer binding 1, normal quaternion alignment, Fibonacci phyllotaxis spirals) -> docs/adr/ADR-0015-gpu-instancing-engine.md
- [project] In-Graph Interactive UI Panel Component System Architecture (PanelComp, SliderPanelComp, ButtonPanelComp, DialPanelComp, TextEntryPanelComp, Flex/Absolute layout, dual channel/texture pipeline) -> docs/adr/ADR-0016-ui-panel-component-system.md
- [project] Advanced Spatial Audio Simulation & Multi-Speaker Ambisonics Engine Architecture (AudioSpatializerChanOp, AmbisonicDecodeChanOp, AudioEmitterComp, AudioListenerComp, VBAP 3D, B-Format WXYZ, 7.1.4 Atmos) -> docs/adr/ADR-0017-spatial-audio-and-ambisonics.md
- [project] Real-Time Physics Engine & Rigid Body Dynamics Integration Architecture (PhysicsWorld 6-DOF solver, SAT collision primitives, PhysicsSolverComp, RigidBodyGeomOp, ColliderGeomOp, PhysicsForceChanOp) -> docs/adr/ADR-0018-real-time-physics-engine.md
- [project] OpenXR / VR & AR Spatial Tracking & Stereoscopic Presentation Architecture (OpenXRRuntime, OpenXRStereoMath, OpenXRHeadsetTexOp, OpenXRControllerChanOp, OpenXRHandTrackingChanOp, OpenXRCameraComp) -> docs/adr/ADR-0019-openxr-spatial-tracking.md
- [project] Volumetric Fog & Real-Time GPU Ray Marching Architecture (VolumetricMath, BeerLambert, HenyeyGreenstein, VolumetricCloudTexOp, LightShaftTexOp, VolumetricFogMatOp, VoxelGridGeomOp) -> docs/adr/ADR-0020-volumetric-fog-and-ray-marching.md
- [project] Laser DAC & ILDA Laser Projector Control Architecture (LaserPoint, LaserEngine, LaserGeomOp, LaserDACChanOp, LaserPatternGeomOp, LaserFileInGeomOp) -> docs/adr/ADR-0021-laser-dac-and-ilda-control.md
- [project] AI Neural Style Transfer & Real-Time ONNX Inference Architecture (ONNXInferenceEngine, PoseKeypoints, NeuralStyleTexOp, ONNXInferenceTexOp, SegmentationMaskTexOp, PoseEstimationChanOp) -> docs/adr/ADR-0022-ai-neural-style-and-inference.md
