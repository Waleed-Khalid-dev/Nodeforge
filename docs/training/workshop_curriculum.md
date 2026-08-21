# Neo Realms — NodeForge Internal Workshop Curriculum

**Course:** Professional Real-Time Media Engineering & Projection Mapping  
**Audience:** Creative Technologists, Technical Directors, Show Programmers, Interactive Artists  
**Prerequisites:** Basic understanding of digital media formats, coordinates, and real-time graphics  
**Duration:** 6 Comprehensive Modules with Hands-On Labs

---

## Course Overview

This internal training manual trains Neo Realms technical artists and engineers to independently design, program, calibrate, and deploy interactive real-time experiences using NodeForge.

---

## Module Breakdown

### Module 1: The NodeForge Mental Model & Directed Acyclic Graphs (DAG)
- **Topics:** NodeForge runtime architecture; Kahn's topological sort; Push-dirty propagation vs. pull-on-demand evaluation; The 6 operator families; Understanding pin data types and color coding.
- **Hands-on Focus:** Building a clean, leak-free starter graph; inspecting dirty states; navigating the infinite canvas with pan/zoom and hotkeys.

### Module 2: Generative Motion Graphics & Shader Pipelines
- **Topics:** GPU Texture pipeline; `FullscreenPass` and `ComputePass`; Texture leasing from `TexturePool`; Separation of Gaussian blur; Modulating shader parameters with Python expressions (`op('lfo1')['chan1']`).
- **Hands-on Lab:** [Lab 1: Generative Motion Graphics & Shader Pipelines](file:///d:/%5BProject%5D/Touch%20Designer/docs/training/lab_01_generative_graphics.md).

### Module 3: 3D Scene Composition, Procedural Geometry & GPU Rasterization
- **Topics:** `GeometryData` interleaved mesh structures; Procedural primitives (`GridGeomOp`, `SphereGeomOp`, `BoxGeomOp`); Vertex noise deformation; Lighting models (`PhongMatOp`, `GLSLMatOp`); Camera perspective math; `RenderTexOp` dynamic rasterizer.
- **Hands-on Lab:** [Lab 2: 3D Scene Composition & Procedural Geometry](file:///d:/%5BProject%5D/Touch%20Designer/docs/training/lab_02_3d_render_pipelines.md).

### Module 4: Multi-Projector Alignment, 2D Bezier Warping & Softedge Blending
- **Topics:** Physical display management; Multi-window presentation routing; 2D Bezier grid warping math; S-curve gamma-correct softedge overlap blending; Black-level compensation; On-site calibration overlay mode (`F11`).
- **Hands-on Lab:** [Lab 3: Multi-Projector Alignment & Softedge Warping](file:///d:/%5BProject%5D/Touch%20Designer/docs/training/lab_03_projection_mapping.md).

### Module 5: Interactive Protocols & Show Control Integration
- **Topics:** Real-time sensor telemetry; Open Sound Control (OSC) binary address decoding; Hardware MIDI CC controller integration; Win32 Overlapped asynchronous Serial communication with microcontrollers; Art-Net 4 DMX512 lighting network broadcast; Python `ScriptDataOp` lifecycle hooks (`onCook`, `onPulse`).
- **Hands-on Lab:** [Lab 4: Protocols, Gestures & Live Show Control](file:///d:/%5BProject%5D/Touch%20Designer/docs/training/lab_04_interactive_controls.md).

### Module 6: Live Performance Profiling, Stability & Standalone Kiosk Deployment
- **Topics:** Sub-microsecond per-node profiling via `CookProfiler`; GPU pass timing with `GpuTimerPool`; Identifying memory leaks and allocation watermarks; Translucent `PerformanceHUD` overlay (`F3`); Headless autostart execution with `nodeforge_player.exe`; Win32 SEH crash report interpretation.
- **Hands-on Focus:** Running 4-hour soak tests, tuning frame pacing budgets, and configuring automatic Windows show autostart scripts.
