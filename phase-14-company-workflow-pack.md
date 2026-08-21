# Implementation Plan — Phase 14: Company Workflow Pack + Training

**Phase:** 14  
**Title:** Company Workflow Pack + Training  
**Status:** In Planning  
**Target Milestone:** M6 (Show-Ready Internal Production Release)  
**Company:** Neo Realms  
**Owner:** Waleed Khalid  

---

## 1. Overview & Objectives

Phase 14 fulfills the core operational goal of NodeForge: delivering production-ready showcase projects, comprehensive operator cheat sheets, hands-on training materials, and enterprise deployment guidelines tailored specifically to Neo Realms' flagship commercial workflows (multi-projector façade mapping, interactive gesture holograms, walkable 3D floor plans, and real-time generative stage visuals).

The goal of this phase is to ensure that any creative engineer or tech artist at Neo Realms can independently build, calibrate, operate, and deploy complex interactive real-time installations using NodeForge with zero dependency on the core engine developers.

---

## 2. Deliverable Scope & Structure

```
d:\[Project]\Touch Designer\
├── samples\
│   ├── 01_facade_mapping\
│   │   ├── facade_mapping.nfp              # Dual-projector façade mapping with Bezier warp & softedge blend
│   │   ├── assets\                         # Alignment test patterns & sample media
│   │   └── README.md                       # Project walkthrough & calibration instructions
│   ├── 02_interactive_floorplan\
│   │   ├── interactive_floorplan.nfp       # 3D walkable floor plan with OSC/gesture tracking & camera orbit
│   │   ├── assets\                         # Floor plan layout CSV & floor textures
│   │   └── README.md                       # Architecture & tracking protocol specs
│   ├── 03_audiovisual_stage\
│   │   ├── audiovisual_stage.nfp           # Generative audio-reactive shader visualizer with MIDI CC control
│   │   ├── assets\                         # Audio stems & texture palettes
│   │   └── README.md                       # Audio analysis & parameter modulation guide
│   └── 04_dmx_showcontrol\
│       ├── dmx_showcontrol.nfp             # Art-Net 4 DMX512 + Serial microcontroller fixture controller
│       ├── assets\                         # Fixture patch tables & cue lists
│       └── README.md                       # Lighting universe routing & serial protocol docs
├── templates\
│   ├── dual_projector_warp_rig.nfc         # Reusable 2-projector warp & softedge component
│   ├── osc_gesture_tracker.nfc             # Reusable OSC multi-point gesture receiver component
│   └── artnet_dmx_chase.nfc                # Reusable Art-Net 4 DMX chase generator component
├── docs\
│   ├── cheat-sheets\
│   │   ├── 00_master_cheat_sheet.md        # Unified shortcut card, signal conventions & Python quickref
│   │   ├── 01_texop_cheat_sheet.md         # 2D Texture ops, GPU shaders & Media I/O (16 operators)
│   │   ├── 02_chanop_cheat_sheet.md        # 1D/SIMD Channels, Audio, Input & Control (17 operators)
│   │   ├── 03_dataop_cheat_sheet.md        # 2D Tables, Text, JSON, Web & Scripting (11 operators)
│   │   ├── 04_geomop_cheat_sheet.md        # 3D Meshes, Primitives & Modifiers (10 operators)
│   │   ├── 05_matop_scene_cheat_sheet.md   # Materials, Shaders, Cameras, Lights & Render (6 operators)
│   │   └── 06_comp_system_cheat_sheet.md   # Containers, In/Out Subnets, Plugins & Projects
│   ├── training\
│   │   ├── workshop_curriculum.md          # 6-module hands-on training manual
│   │   ├── lab_01_generative_graphics.md   # Lab 1: Generative Motion Graphics & Shader Pipelines
│   │   ├── lab_02_3d_render_pipelines.md   # Lab 2: 3D Scene Composition & Procedural Geometry
│   │   ├── lab_03_projection_mapping.md    # Lab 3: Multi-Projector Alignment & Softedge Warping
│   │   └── lab_04_interactive_controls.md  # Lab 4: Protocols, Gestures & Live Show Control
│   └── deployment\
│       ├── it_deployment_guide.md          # Enterprise IT staging, air-gapped runtime & kiosk autostart
│       └── on_site_calibration_checklist.md # On-site setup, EDID lock & calibration procedure
└── tests\
    └── workflow_pack_test.cpp              # Automated GoogleTest suite validating all sample projects
```

---

## 3. Detailed Task Breakdown

### Task 14.1: Production Sample Projects (`samples/`) & Reusable Components (`templates/`)
- **Agent:** `backend-specialist` / `project-planner`
- **Skills:** `clean-code`, `plan-writing`
- **Priority:** P0 (Core Foundation)
- **Input:** Existing `.nfp` JSON v1 schema, node registry, and operator specifications.
- **Output:** 
  1. `samples/01_facade_mapping/facade_mapping.nfp` + README + assets
  2. `samples/02_interactive_floorplan/interactive_floorplan.nfp` + README + assets
  3. `samples/03_audiovisual_stage/audiovisual_stage.nfp` + README + assets
  4. `samples/04_dmx_showcontrol/dmx_showcontrol.nfp` + README + assets
  5. `templates/dual_projector_warp_rig.nfc`, `templates/osc_gesture_tracker.nfc`, `templates/artnet_dmx_chase.nfc`
- **Verify:** Every `.nfp` and `.nfc` file parses cleanly into valid JSON and represents a valid NodeForge network.

### Task 14.2: Operator Family Cheat Sheets (`docs/cheat-sheets/`)
- **Agent:** `project-planner` / `documentation-templates`
- **Skills:** `documentation-templates`, `clean-code`
- **Priority:** P1 (Essential Documentation)
- **Input:** 68 operator specifications in `docs/operator-spec/` and ADRs.
- **Output:**
  1. `docs/cheat-sheets/00_master_cheat_sheet.md` (Hotkeys, canvas gestures, type colors, Python expressions `op('name')['chan']`, performance guidelines)
  2. `docs/cheat-sheets/01_texop_cheat_sheet.md` (TexOp pipeline, pins, formats, shader uniforms, Spout/NDI/Warp)
  3. `docs/cheat-sheets/02_chanop_cheat_sheet.md` (ChanOp buffer layout, SIMD math, LFO waveforms, time-slicing, MIDI/OSC/Art-Net)
  4. `docs/cheat-sheets/03_dataop_cheat_sheet.md` (DataTable 2D grid, CSV/TSV, JSON Pointer queries, Python hooks `onCook`/`onPulse`, Serial port)
  5. `docs/cheat-sheets/04_geomop_cheat_sheet.md` (GeometryData vertex attributes, primitives, noise modifiers, normals calculation)
  6. `docs/cheat-sheets/05_matop_scene_cheat_sheet.md` (Phong/Constant/GLSL shading, push constants, CameraComp projection, LightComp, RenderTexOp)
  7. `docs/cheat-sheets/06_comp_system_cheat_sheet.md` (ContainerComp encapsulation, InOp/OutOp boundary routing, `.nfc` components, PluginProxyNode)
- **Verify:** Every operator currently in the engine is documented with exact pin names, types, parameters, and code examples.

### Task 14.3: Internal Workshop Training Curriculum & Practical Labs (`docs/training/`)
- **Agent:** `project-planner`
- **Skills:** `documentation-templates`, `brainstorming`
- **Priority:** P1 (Knowledge Transfer)
- **Input:** Company flagship use cases & engine architecture.
- **Output:**
  1. `docs/training/workshop_curriculum.md` (6 Modules: Mental Model & DAG, Generative Shaders, 3D & Instancing, Projection Mapping & Softedge, Interactive Protocols, Live Performance & Kiosk Player)
  2. `docs/training/lab_01_generative_graphics.md` (Hands-on exercise: Building an animated generative audio-reactive ripple shader from scratch)
  3. `docs/training/lab_02_3d_render_pipelines.md` (Hands-on exercise: Constructing a 3D procedural geometric landscape with camera controls)
  4. `docs/training/lab_03_projection_mapping.md` (Hands-on exercise: Configuring dual-screen output, setting up 2D Bezier warp grid, and tuning S-curve softedge overlap)
  5. `docs/training/lab_04_interactive_controls.md` (Hands-on exercise: Binding OSC/MIDI inputs to visual parameters and triggering DMX lighting cues)
- **Verify:** Clear step-by-step instructions that can be followed without prior codebase knowledge.

### Task 14.4: IT & Field Deployment Operations Manual (`docs/deployment/`)
- **Agent:** `project-planner` / `deployment-procedures`
- **Skills:** `deployment-procedures`, `clean-code`
- **Priority:** P1 (Operations & Reliability)
- **Input:** Kiosk player architecture, crash reporter, hardware targets.
- **Output:**
  1. `docs/deployment/it_deployment_guide.md` (Hardware specifications, GPU driver locks, Windows power/display settings, air-gapped staging, `nodeforge_player.exe` headless flags, autostart service config, firewall port tables, crash log diagnostics)
  2. `docs/deployment/on_site_calibration_checklist.md` (Step-by-step on-site projection alignment and show sign-off checklist)
- **Verify:** Complete operational runbook covers all failure modes, restart procedures, and diagnostic steps.

### Task 14.5: Automated Workflow Pack C++ Verification Suite (`tests/workflow_pack_test.cpp`)
- **Agent:** `test-engineer` / `backend-specialist`
- **Skills:** `testing-patterns`, `verify-changes`
- **Priority:** P0 (Quality & CI Gate)
- **Input:** Sample `.nfp` files, `ProjectSerializer`, `Graph`, `CookContext`, `NodeRegistry`.
- **Output:**
  1. `tests/workflow_pack_test.cpp` (Automated GoogleTest suite testing project deserialization, full graph instantiation, pin connectivity integrity, and multi-frame evaluation without errors for all 4 flagship samples)
  2. Update `CMakeLists.txt` to include `workflow_pack_test` in test targets.
- **Verify:** `ctest --output-on-failure -R WorkflowPack` runs and passes 100% with 0 memory leaks.

---

## 4. Phase X: Final Verification Checklist

- [ ] All 4 flagship sample project directories and `.nfp` files created in `samples/`
- [ ] All 3 reusable `.nfc` components created in `templates/`
- [ ] All 7 cheat sheets created in `docs/cheat-sheets/`
- [ ] Complete workshop training curriculum + 4 hands-on labs created in `docs/training/`
- [ ] Enterprise IT deployment guide + on-site checklist created in `docs/deployment/`
- [ ] `tests/workflow_pack_test.cpp` written and added to `CMakeLists.txt`
- [ ] Automated tests compile and pass with 100% success rate
- [ ] `STATUS.md` updated to mark Phase 14 complete with signed-off DoD
- [ ] Session memory updated (`.agent/memory/MEMORY.md`)
