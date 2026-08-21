# Implementation Plan — Phase 15 Epic 15.6: OpenXR / VR & AR Spatial Tracking System

**Phase:** 15 (Expansion Toward “Almost Everything”)  
**Epic:** 15.6 (OpenXR / VR & AR Spatial Tracking System — HMD Display & Controller CHOP)  
**Status:** In Planning  
**Target Milestone:** M7 (Advanced Visual FX & Dynamic Tools)  
**Company:** Neo Realms  
**Owner:** Waleed Khalid  

---

## 1. Overview & Objectives

Epic 15.6 introduces an enterprise-grade **OpenXR / VR & AR Spatial Tracking & Stereoscopic Presentation Subsystem** to NodeForge. Built for tethered/wireless VR headsets (Meta Quest, HTC Vive, Valve Index, Apple Vision Pro OpenXR streaming) and interactive spatial computing installations.

Key capabilities:
- **`OpenXRRuntime` (`src/xr/OpenXRRuntime.h/.cpp`)**: Native OpenXR 1.0 runtime wrapper managing session lifecycle (`XrInstance, XrSession, XrSpace, XrSwapchain`), asymmetric stereoscopic FOV projection matrices, IPD (interpupillary distance) offsets, and automatic headless/CI emulation fallback.
- **`OpenXRHeadsetTexOp` (Family: `TexOp`)**: Dual-eye stereoscopic presenter rendering Left and Right eye views into OpenXR runtime swapchains, with FOV/IPD adjustment, guardian boundary passthrough, and companion 2D preview window output.
- **`OpenXRControllerChanOp` (Family: `ChanOp`)**: Streams 6-DOF tracking transforms (`tx, ty, tz, rx, ry, rz, rw`) for Left and Right controllers, button inputs (`trigger, grip, primary, secondary, thumbstick_xy`), and battery levels.
- **`OpenXRHandTrackingChanOp` (Family: `ChanOp`)**: Streams 26 skeletal hand joint transforms per hand (Wrist, Thumb, Index, Middle, Ring, Pinky tip/distal/intermediate/proximal joints) and pinch gesture metrics.
- **`OpenXRCameraComp` (Family: `Comp`)**: Stereoscopic camera rig synchronized with HMD head pose in 3D world space.
- **Cross-Subsystem Synergy**: HMD head pose automatically drives `AudioListenerComp` for 3D spatial binaural audio; hand controllers inject physics impulses via `PhysicsForceChanOp`, attract/repel GPU compute particle clouds (`ParticleAttractorGeomOp`), and cast virtual laser pointers to interact with in-world UI panels (`PanelComp`).

---

## 2. Architecture & Stereoscopic Projection Model (ADR-0019)

### 2.1 Asymmetric FOV Projection Matrix
For each eye $e \in \{\text{Left}, \text{Right}\}$ with OpenXR field-of-view angles $(\text{angleLeft}, \text{angleRight}, \text{angleUp}, \text{angleDown})$ and near/far clipping planes $(z_{\text{near}}, z_{\text{far}})$:

$$\begin{aligned}
\tan L &= \tan(\text{angleLeft}), \quad \tan R = \tan(\text{angleRight}) \\
\tan U &= \tan(\text{angleUp}), \quad \tan D = \tan(\text{angleDown}) \\
\mathbf{P}[0, 0] &= \frac{2}{\tan R - \tan L}, \quad \mathbf{P}[2, 0] = \frac{\tan R + \tan L}{\tan R - \tan L} \\
\mathbf{P}[1, 1] &= \frac{2}{\tan U - \tan D}, \quad \mathbf{P}[2, 1] = \frac{\tan U + \tan D}{\tan U - \tan D} \\
\mathbf{P}[2, 2] &= -\frac{z_{\text{far}} + z_{\text{near}}}{z_{\text{far}} - z_{\text{near}}}, \quad \mathbf{P}[3, 2] = -\frac{2 \cdot z_{\text{far}} \cdot z_{\text{near}}}{z_{\text{far}} - z_{\text{near}}} \\
\mathbf{P}[2, 3] &= -1.0, \quad \mathbf{P}[3, 3] = 0.0
\end{aligned}$$

### 2.2 View Matrix with IPD Offset
For head pose $(\vec{P}_{\text{head}}, \mathbf{Q}_{\text{head}})$ and eye offset vector $\vec{O}_e = (\pm \text{IPD}/2, 0, 0)$:

$$\begin{aligned}
\vec{P}_e &= \vec{P}_{\text{head}} + \mathbf{Q}_{\text{head}} \cdot \vec{O}_e \\
\mathbf{V}_e &= (\mathbf{R}(\mathbf{Q}_{\text{head}}))^{-1} \cdot \mathbf{T}(-\vec{P}_e)
\end{aligned}$$

---

## 3. Operator Suite & Specifications

### 1. `OpenXRHeadsetTexOp` (Family: `TexOp`)
- **Input Pins:** `in_left_eye` (Tex), `in_right_eye` (Tex).
- **Output Pins:** `output` (Tex, companion 2D preview).
- **Parameters:**
  - `stereo_mode`: `0: Side-by-Side`, `1: Dual Swapchain Present`.
  - `ipd_mm`: `float` (default: `64.0`, range: `50.0 .. 80.0`).
  - `mirror_window`: `bool` (default: `true`).
  - `enable_passthrough`: `bool` (default: `false`).

### 2. `OpenXRControllerChanOp` (Family: `ChanOp`)
- **Input Pins:** None.
- **Output Pins:** `out_left` (Chan, left controller channels), `out_right` (Chan, right controller channels).
- **Parameters:**
  - `hand_select`: `0: Both`, `1: Left Only`, `2: Right Only`.
  - `haptic_pulse`: `float` (default: `0.0`, range: `0.0 .. 1.0`).

### 3. `OpenXRHandTrackingChanOp` (Family: `ChanOp`)
- **Input Pins:** None.
- **Output Pins:** `out_skeleton` (Chan, 26 joint XYZ + orientation streams), `out_gestures` (Chan, pinch strength, grab strength, palm normal).
- **Parameters:**
  - `hand_select`: `0: Both`, `1: Left`, `2: Right`.
  - `confidence_filter`: `float` (default: `0.5`).

### 4. `OpenXRCameraComp` (Family: `Comp`)
- **Input Pins:** None.
- **Output Pins:** `output` (Comp), `out_head_pose` (Chan, `tx, ty, tz, rx, ry, rz, rw`).
- **Parameters:**
  - `tracking_space`: `0: Local (Seated)`, `1: Stage (Room-Scale)`.
  - `camera_offset`: `vec3` (default: `[0.0, 0.0, 0.0]`).

---

## 4. Planned File Structure

```
d:\[Project]\Touch Designer\
├── docs\
│   ├── adr\
│   │   └── ADR-0019-openxr-spatial-tracking.md
│   ├── operator-spec\
│   │   ├── OpenXRHeadsetTexOp.md
│   │   ├── OpenXRControllerChanOp.md
│   │   ├── OpenXRHandTrackingChanOp.md
│   │   └── OpenXRCameraComp.md
│   └── cheat-sheets\
│       └── 12_openxr_cheat_sheet.md
├── src\
│   ├── xr\
│   │   ├── OpenXRRuntime.h/.cpp        # OpenXR 1.0 lifecycle, session, and emulation
│   │   └── OpenXRStereoMath.h/.cpp     # Asymmetric FOV & IPD stereo matrices
│   └── operators\
│       ├── tex\
│       │   └── OpenXRHeadsetTexOp.h/.cpp
│       ├── chan\
│       │   ├── OpenXRControllerChanOp.h/.cpp
│       │   └── OpenXRHandTrackingChanOp.h/.cpp
│       └── comp\
│           └── OpenXRCameraComp.h/.cpp
├── samples\
│   └── 10_openxr_immersive_holodeck\
│       ├── openxr_immersive_holodeck.nfp # Flagship VR holodeck project
│       └── README.md
└── tests\
    ├── unit\
    │   └── openxr_system_test.cpp      # Unit tests for stereo math, tracking, emulation
    └── benchmark\
        └── openxr_benchmark.cpp        # 90/120 FPS stereo render throughput benchmark
```

---

## 5. Detailed Task Breakdown

### Task 15.6.1: Architecture Documentation & Operator Specifications
- **Agent:** `project-planner` / `architecture`
- **Priority:** P0 (Design Pre-requisite)
- **Output:**
  1. `docs/adr/ADR-0019-openxr-spatial-tracking.md`
  2. `docs/operator-spec/OpenXRHeadsetTexOp.md`
  3. `docs/operator-spec/OpenXRControllerChanOp.md`
  4. `docs/operator-spec/OpenXRHandTrackingChanOp.md`
  5. `docs/operator-spec/OpenXRCameraComp.md`
  6. `docs/cheat-sheets/12_openxr_cheat_sheet.md`

### Task 15.6.2: OpenXR Runtime Layer & Operator Implementations
- **Agent:** `backend-specialist` / `game-developer`
- **Priority:** P0 (Core Implementation)
- **Output:**
  1. `src/xr/OpenXRStereoMath.h/.cpp`
  2. `src/xr/OpenXRRuntime.h/.cpp`
  3. `src/operators/tex/OpenXRHeadsetTexOp.h/.cpp`
  4. `src/operators/chan/OpenXRControllerChanOp.h/.cpp`
  5. `src/operators/chan/OpenXRHandTrackingChanOp.h/.cpp`
  6. `src/operators/comp/OpenXRCameraComp.h/.cpp`
  7. Register all operators in `src/graph/CoreNodes.cpp` and `CMakeLists.txt`.

### Task 15.6.3: Showcase Sample Project (`samples/10_openxr_immersive_holodeck/`)
- **Agent:** `project-planner` / `backend-specialist`
- **Priority:** P1 (Production Sample)
- **Output:**
  1. `samples/10_openxr_immersive_holodeck/openxr_immersive_holodeck.nfp`
  2. `samples/10_openxr_immersive_holodeck/README.md`

### Task 15.6.4: Automated Unit Tests & 90/120 FPS Benchmark
- **Agent:** `test-engineer` / `backend-specialist`
- **Priority:** P0 (CI Quality Gate)
- **Output:**
  1. `tests/unit/openxr_system_test.cpp`
  2. `tests/benchmark/openxr_benchmark.cpp`
  3. Update `tests/CMakeLists.txt`.

---

## 6. Phase X: Final Verification Checklist

- [ ] `docs/adr/ADR-0019-openxr-spatial-tracking.md` written and approved
- [ ] Operator specs for all 4 OpenXR operators created in `docs/operator-spec/`
- [ ] `OpenXRRuntime`, `OpenXRStereoMath`, `OpenXRHeadsetTexOp`, `OpenXRControllerChanOp`, `OpenXRHandTrackingChanOp`, `OpenXRCameraComp` implemented
- [ ] Operators registered in `CoreNodes.cpp` and `CMakeLists.txt`
- [ ] `samples/10_openxr_immersive_holodeck/openxr_immersive_holodeck.nfp` created with documentation
- [ ] `tests/unit/openxr_system_test.cpp` and `tests/benchmark/openxr_benchmark.cpp` added to CMake
- [ ] 90/120 FPS stereo benchmark verified with zero memory leaks
- [ ] All automated tests passing (100% pass rate)
- [ ] `STATUS.md` and `.agent/memory/MEMORY.md` updated
