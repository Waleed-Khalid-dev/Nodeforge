# Implementation Plan — Phase 15 Epic 15.4: Advanced Spatial Audio Simulation & Multi-Speaker Ambisonics Engine

**Phase:** 15 (Expansion Toward “Almost Everything”)  
**Epic:** 15.4 (Advanced Spatial Audio Simulation & Multi-Speaker Ambisonics Engine)  
**Status:** In Planning  
**Target Milestone:** M7 (Advanced Visual FX & Dynamic Tools)  
**Company:** Neo Realms  
**Owner:** Waleed Khalid  

---

## 1. Overview & Objectives

Epic 15.4 introduces an enterprise-grade **3D Spatial Audio & Multi-Speaker Ambisonics Engine** to NodeForge. Engineered for immersive planetarium domes, multi-channel surround sound auditoriums (5.1, 7.1.4 Dolby Atmos ceiling setups), VR audio installations, and interactive gallery spaces.

Key capabilities:
- **`AudioSpatializerChanOp` (Family: `ChanOp`)**: 3D sound source panner taking audio streams and 3D emitter/listener coordinates, calculating real-time discrete multi-speaker gains (VBAP 2D/3D), distance falloff attenuation $[1 / (1 + \alpha \cdot d)]$, high-frequency air absorption, and Doppler pitch shifting. Emits multi-channel discrete speaker feeds (up to 16 channels) or First-Order Ambisonics ($W, X, Y, Z$).
- **`AmbisonicDecodeChanOp` (Family: `ChanOp`)**: Decodes 4-channel B-Format soundfields ($W, X, Y, Z$) into target physical speaker topologies (Stereo, Quad, 5.1, 7.1.4 Atmos dome, 16-channel array, or custom speaker coordinates table).
- **`AudioEmitterComp` (Family: `Comp`)**: 3D scene spatial sound emitter binding audio channels to 3D geometry objects in the scene graph with radius of influence, cone directivity, and obstruction falloff.
- **`AudioListenerComp` (Family: `Comp`)**: 3D scene virtual listener / microphone representing the observer's head position and orientation in 3D space.
- **Planar Multi-Channel Streaming**: High-throughput vectorized audio buffering in `ChannelBuffer` with zero memory reallocations during graph cook cycles.

---

## 2. Architecture & Mathematical Foundation (ADR-0017)

### 2.1 First-Order Ambisonics (B-Format)
For a sound source with signal $S(t)$, azimuth angle $\theta$, and elevation angle $\phi$:

$$\begin{aligned}
W(t) &= \frac{S(t)}{\sqrt{2}} \quad \text{(Omnidirectional Pressure Component)} \\
X(t) &= S(t) \cdot \cos\theta \cdot \cos\phi \quad \text{(Front-Back Figure-8 Velocity)} \\
Y(t) &= S(t) \cdot \sin\theta \cdot \cos\phi \quad \text{(Left-Right Figure-8 Velocity)} \\
Z(t) &= S(t) \cdot \sin\phi \quad \text{(Up-Down Figure-8 Velocity)}
\end{aligned}$$

### 2.2 Ambisonic Decoding to $N$ Physical Speakers
For a speaker $i$ at azimuth $\theta_i$ and elevation $\phi_i$:

$$G_i(t) = \frac{1}{N} \left[ W(t) \sqrt{2} + X(t) \cos\theta_i \cos\phi_i + Y(t) \sin\theta_i \cos\phi_i + Z(t) \sin\phi_i \right]$$

### 2.3 Acoustic Physics & Attenuation
- **Distance Attenuation:** $A(d) = \frac{1}{1 + \alpha \cdot d}$ where $d = \|\vec{P}_{\text{emitter}} - \vec{P}_{\text{listener}}\|$.
- **Cone Directivity:** Attenuation based on emitter forward vector $\vec{F}$ and angle $\gamma = \arccos(\hat{F} \cdot \hat{D})$.
- **Doppler Effect:** Frequency pitch shift based on relative emitter/listener velocity $\Delta v$.

---

## 3. Operator Suite & Specifications

### 1. `AudioSpatializerChanOp` (Family: `ChanOp`)
- **Input Pins:** `in_audio` (Chan, monophonic/stereo source), `in_pos` (Chan, optional dynamic 3D coordinates `x, y, z`).
- **Output Pins:** `out_audio` (Chan, multi-channel spatialized output: Stereo, 5.1, 7.1.4, or Ambisonic $W, X, Y, Z$).
- **Parameters:**
  - `spatial_mode`: `0: Ambisonics B-Format`, `1: VBAP Multi-Speaker Layout`.
  - `speaker_layout`: `0: Stereo`, `1: Quad 4.0`, `2: 5.1 Surround`, `3: 7.1.4 Atmos Ceiling`, `4: Octaphonic 8.0`, `5: 16-Ch Array`.
  - `emitter_pos`: `vec3` source position (default: `[0.0, 0.0, 0.0]`).
  - `listener_pos`: `vec3` listener position (default: `[0.0, 0.0, 0.0]`).
  - `listener_rot`: `vec3` listener rotation pitch/yaw/roll (default: `[0.0, 0.0, 0.0]`).
  - `attenuation_rolloff`: `float` distance falloff coefficient (default: `1.0`).
  - `max_distance`: `float` audible distance limit (default: `100.0`).
  - `doppler_factor`: `float` Doppler pitch shift intensity (default: `1.0`).

### 2. `AmbisonicDecodeChanOp` (Family: `ChanOp`)
- **Input Pins:** `in_bformat` (Chan, 4-channel $W, X, Y, Z$ stream), `speaker_layout_data` (Data, optional custom speaker XYZ table).
- **Output Pins:** `out_speakers` (Chan, discrete multi-channel audio stream).
- **Parameters:**
  - `target_layout`: `0: Stereo`, `1: Quad 4.0`, `2: 5.1 Surround`, `3: 7.1.4 Atmos Dome`, `4: Custom Table`.
  - `shelf_filter_hf`: `bool` high-frequency shelf optimization for human head acoustics (default: `true`).

### 3. `AudioEmitterComp` (Family: `Comp`)
- **Input Pins:** `in_audio` (Chan, input audio stream), `in_geom` (Geom, optional mesh to attach sound source to).
- **Output Pins:** `output` (Comp), `out_audio` (Chan).
- **Parameters:**
  - `translate`: `vec3` world position.
  - `radius`: `float` inner full-volume radius (default: `1.0`).
  - `cone_inner_angle`: `float` directional beam inner angle in degrees (default: `360.0`).
  - `cone_outer_gain`: `float` gain outside directional beam (default: `0.0`).

### 4. `AudioListenerComp` (Family: `Comp`)
- **Input Pins:** `in_cam` (Comp, optional CameraComp to sync listener position & orientation).
- **Output Pins:** `output` (Comp), `out_state` (Chan, `pos_x, pos_y, pos_z, yaw, pitch`).
- **Parameters:**
  - `position`: `vec3` listener world coordinates.
  - `orientation`: `vec3` listener look orientation.

---

## 4. Planned File Structure

```
d:\[Project]\Touch Designer\
├── docs\
│   ├── adr\
│   │   └── ADR-0017-spatial-audio-and-ambisonics.md
│   ├── operator-spec\
│   │   ├── AudioSpatializerChanOp.md
│   │   ├── AmbisonicDecodeChanOp.md
│   │   ├── AudioEmitterComp.md
│   │   └── AudioListenerComp.md
│   └── cheat-sheets\
│       └── 10_spatial_audio_cheat_sheet.md
├── src\
│   └── operators\
│       ├── chan\
│       │   ├── AudioSpatializerChanOp.h/.cpp
│       │   └── AmbisonicDecodeChanOp.h/.cpp
│       └── comp\
│           ├── AudioEmitterComp.h/.cpp
│           └── AudioListenerComp.h/.cpp
├── samples\
│   └── 08_spatial_audio_dome\
│       ├── spatial_audio_dome.nfp  # Flagship planetarium spatial audio project
│       └── README.md
└── tests\
    ├── unit\
    │   └── spatial_audio_test.cpp  # Unit tests for VBAP, Ambisonics, and Doppler
    └── benchmark\
        └── spatial_audio_benchmark.cpp # 64-stream 16-channel throughput benchmark
```

---

## 5. Detailed Task Breakdown

### Task 15.4.1: Architecture Documentation & Operator Specifications
- **Agent:** `project-planner` / `architecture`
- **Priority:** P0 (Design Pre-requisite)
- **Output:**
  1. `docs/adr/ADR-0017-spatial-audio-and-ambisonics.md`
  2. `docs/operator-spec/AudioSpatializerChanOp.md`
  3. `docs/operator-spec/AmbisonicDecodeChanOp.md`
  4. `docs/operator-spec/AudioEmitterComp.md`
  5. `docs/operator-spec/AudioListenerComp.md`
  6. `docs/cheat-sheets/10_spatial_audio_cheat_sheet.md`

### Task 15.4.2: Spatial Audio Operator Implementations
- **Agent:** `backend-specialist` / `orchestrator`
- **Priority:** P0 (Core Foundation)
- **Output:**
  1. `src/operators/chan/AudioSpatializerChanOp.h/.cpp`
  2. `src/operators/chan/AmbisonicDecodeChanOp.h/.cpp`
  3. `src/operators/comp/AudioEmitterComp.h/.cpp`
  4. `src/operators/comp/AudioListenerComp.h/.cpp`
  5. Register all 4 operators in `src/graph/CoreNodes.cpp` and `CMakeLists.txt`.

### Task 15.4.3: Showcase Sample Project (`samples/08_spatial_audio_dome/`)
- **Agent:** `project-planner` / `backend-specialist`
- **Priority:** P1 (Production Sample)
- **Output:**
  1. `samples/08_spatial_audio_dome/spatial_audio_dome.nfp`
  2. `samples/08_spatial_audio_dome/README.md`

### Task 15.4.4: Automated Unit Tests & 64-Stream Benchmark
- **Agent:** `test-engineer` / `backend-specialist`
- **Priority:** P0 (CI Quality Gate)
- **Output:**
  1. `tests/unit/spatial_audio_test.cpp`
  2. `tests/benchmark/spatial_audio_benchmark.cpp`
  3. Update `tests/CMakeLists.txt`.

---

## 6. Phase X: Final Verification Checklist

- [ ] `docs/adr/ADR-0017-spatial-audio-and-ambisonics.md` written and approved
- [ ] Operator specs for all 4 spatial audio operators created in `docs/operator-spec/`
- [ ] `AudioSpatializerChanOp`, `AmbisonicDecodeChanOp`, `AudioEmitterComp`, `AudioListenerComp` implemented
- [ ] Operators registered in `CoreNodes.cpp` and `CMakeLists.txt`
- [ ] `samples/08_spatial_audio_dome/spatial_audio_dome.nfp` created with documentation
- [ ] `tests/unit/spatial_audio_test.cpp` and `tests/benchmark/spatial_audio_benchmark.cpp` added to CMake
- [ ] 64-stream 16-channel spatial audio benchmark verified with sub-millisecond cook times and zero memory leaks
- [ ] All automated tests passing (100% pass rate)
- [ ] `STATUS.md` and `.agent/memory/MEMORY.md` updated
