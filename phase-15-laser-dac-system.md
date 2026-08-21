# Implementation Plan — Phase 15 Epic 15.8: Laser DAC & ILDA Laser Projector Control

**Phase:** 15 (Expansion Toward “Almost Everything”)  
**Epic:** 15.8 (Laser DAC & ILDA Laser Projector Control — Ether-Dream / Helios DAC CHOP & LaserSOP)  
**Status:** In Planning  
**Target Milestone:** M7 (Advanced Visual FX & Dynamic Tools)  
**Company:** Neo Realms  
**Owner:** Waleed Khalid  

---

## 1. Overview & Objectives

Epic 15.8 introduces an enterprise-grade **Laser DAC & ILDA Vector Projector Control Subsystem** to NodeForge. Engineered for live concert laser shows, architectural laser mapping, kinetic beam sculptures, and vector graphics.

Key capabilities:
- **`LaserEngine` & `LaserPoint` (`src/laser/LaserEngine.h/.cpp`)**: Core data structures and algorithms for 16-bit ILDA $(X, Y, Z, R, G, B, I, \text{flags})$ vector points, galvo scan path optimization (blanking point insertion, corner dwell delays, velocity limiting), and DAC buffer management (12 kpps to 100 kpps).
- **`LaserGeomOp` (Family: `GeomOp`)**: Converts 3D meshes (`LineGeomOp`, `TextGeomOp`, `GridGeomOp`, `CircleGeomOp`) or 2D vector contours into an optimized ILDA laser point path with controllable blanking delays, corner dwells, and target scan rates.
- **`LaserDACChanOp` (Family: `ChanOp`)**: Streams ILDA point channels (`x, y, r, g, b, intensity, blank`) to hardware DACs (Ether Dream / Helios USB) with real-time buffer health telemetry and emergency safety shutter control.
- **`LaserPatternGeomOp` (Family: `GeomOp`)**: Generates classic laser show geometric lissajous curves, beam fans, spirographs, and geometric abstract tunnels.
- **`LaserFileInGeomOp` (Family: `GeomOp`)**: Loads standard ILDA vector animation files (`.ild` format).
- **Cross-Subsystem Synergy**: Laser vectors render into 3D volumetric fog screens (`VolumetricFogMatOp`), audio waveforms deform lissajous beam paths (`AudioFileInChanOp`), and DMX consoles / in-graph UI panels trigger laser safety shutters and color banks (`PanelComp`).

---

## 2. Architecture & Galvo Scanning Model (ADR-0021)

### 2.1 ILDA Point Structure & Normalization
ILDA standard 16-bit signed coordinate space: $X, Y, Z \in [-32768, 32767]$, $R, G, B, I \in [0.0, 1.0]$:

$$\begin{aligned}
X_{16} &= \text{clamp}\left(\text{round}\left(x_{\text{norm}} \times 32767\right), -32768, 32767\right) \\
Y_{16} &= \text{clamp}\left(\text{round}\left(y_{\text{norm}} \times 32767\right), -32768, 32767\right) \\
\text{Flags} &= \text{isBlanked} \,?\, 0x40 \,:\, 0x00
\end{aligned}$$

### 2.2 Galvo Path Optimization
- **Blanking Transition:** When jumping between disconnected contours, insert $N_{\text{pre}}$ blanked points at the departure vertex, $N_{\text{travel}}$ transit points, and $N_{\text{post}}$ blanked points at the arrival vertex.
- **Corner Dwell:** At acute corner angles $\theta < \theta_{\text{thresh}}$, insert $N_{\text{dwell}}$ duplicated points to allow physical galvanometer mirrors to settle before accelerating along the next segment.

---

## 3. Operator Suite & Specifications

### 1. `LaserGeomOp` (Family: `GeomOp`)
- **Input Pins:** `geometry` (Geom).
- **Output Pins:** `output` (Geom, optimized laser point path), `out_points` (Chan, raw DAC point streams).
- **Parameters:**
  - `target_pps`: `int` (default: `30000`, range: `10000 .. 100000`).
  - `blank_delay`: `int` (default: `8`, range: `0 .. 32`).
  - `corner_dwell`: `int` (default: `4`, range: `0 .. 16`).
  - `color_override`: `vec4` (default: `[0.0, 1.0, 0.2, 1.0]`).

### 2. `LaserDACChanOp` (Family: `ChanOp`)
- **Input Pins:** `in_points` (Chan, `x, y, r, g, b, blank`).
- **Output Pins:** `out_telemetry` (Chan, buffer fill level, current pps, dropped points).
- **Parameters:**
  - `dac_type`: `0: Ether Dream (Network)`, `1: Helios (USB)`, `2: Emulated Headless`.
  - `ip_address`: `string` (default: `"192.168.1.100"`).
  - `safety_shutter`: `bool` (default: `true`).
  - `master_brightness`: `float` (default: `1.0`, range: `0.0 .. 1.0`).

### 3. `LaserPatternGeomOp` (Family: `GeomOp`)
- **Input Pins:** None.
- **Output Pins:** `output` (Geom).
- **Parameters:**
  - `pattern_type`: `0: Lissajous Curve`, `1: Spirograph`, `2: Multi-Beam Fan`, `3: Geometric Tunnel`.
  - `frequency_a`: `float` (default: `3.0`).
  - `frequency_b`: `float` (default: `2.0`).
  - `phase`: `float` (default: `0.0`).
  - `beam_count`: `int` (default: `8`, range: `2 .. 64`).

### 4. `LaserFileInGeomOp` (Family: `GeomOp`)
- **Input Pins:** None.
- **Output Pins:** `output` (Geom).
- **Parameters:**
  - `file_path`: `string` (default: `""`).
  - `play_mode`: `0: Loop`, `1: Hold`, `2: Scrub`.
  - `speed`: `float` (default: `1.0`).

---

## 4. Planned File Structure

```
d:\[Project]\Touch Designer\
├── docs\
│   ├── adr\
│   │   └── ADR-0021-laser-dac-and-ilda-control.md
│   ├── operator-spec\
│   │   ├── LaserGeomOp.md
│   │   ├── LaserDACChanOp.md
│   │   ├── LaserPatternGeomOp.md
│   │   └── LaserFileInGeomOp.md
│   └── cheat-sheets\
│       └── 14_laser_cheat_sheet.md
├── src\
│   ├── laser\
│   │   ├── LaserPoint.h
│   │   └── LaserEngine.h/.cpp          # Galvo optimization, ILDA packing, DAC buffer
│   └── operators\
│       ├── geom\
│       │   ├── LaserGeomOp.h/.cpp
│       │   ├── LaserPatternGeomOp.h/.cpp
│       │   └── LaserFileInGeomOp.h/.cpp
│       └── chan\
│           └── LaserDACChanOp.h/.cpp
├── samples\
│   └── 12_laser_symphony_spectacular\
│       ├── laser_symphony_spectacular.nfp # Flagship laser show project
│       └── README.md
└── tests\
    ├── unit\
    │   └── laser_system_test.cpp       # Unit tests for ILDA points, galvo optimization, patterns
    └── benchmark\
        └── laser_benchmark.cpp         # 100 kpps DAC streaming throughput benchmark
```

---

## 5. Detailed Task Breakdown

### Task 15.8.1: Architecture Documentation & Operator Specifications
- **Agent:** `project-planner` / `architecture`
- **Priority:** P0 (Design Pre-requisite)
- **Output:**
  1. `docs/adr/ADR-0021-laser-dac-and-ilda-control.md`
  2. `docs/operator-spec/LaserGeomOp.md`
  3. `docs/operator-spec/LaserDACChanOp.md`
  4. `docs/operator-spec/LaserPatternGeomOp.md`
  5. `docs/operator-spec/LaserFileInGeomOp.md`
  6. `docs/cheat-sheets/14_laser_cheat_sheet.md`

### Task 15.8.2: Laser Engine Core & Operator Implementations
- **Agent:** `backend-specialist` / `game-developer`
- **Priority:** P0 (Core Implementation)
- **Output:**
  1. `src/laser/LaserPoint.h`
  2. `src/laser/LaserEngine.h/.cpp`
  3. `src/operators/geom/LaserGeomOp.h/.cpp`
  4. `src/operators/geom/LaserPatternGeomOp.h/.cpp`
  5. `src/operators/geom/LaserFileInGeomOp.h/.cpp`
  6. `src/operators/chan/LaserDACChanOp.h/.cpp`
  7. Register all operators in `src/graph/CoreNodes.cpp` and `CMakeLists.txt`.

### Task 15.8.3: Showcase Sample Project (`samples/12_laser_symphony_spectacular/`)
- **Agent:** `project-planner` / `backend-specialist`
- **Priority:** P1 (Production Sample)
- **Output:**
  1. `samples/12_laser_symphony_spectacular/laser_symphony_spectacular.nfp`
  2. `samples/12_laser_symphony_spectacular/README.md`

### Task 15.8.4: Automated Unit Tests & 100 kpps Benchmark
- **Agent:** `test-engineer` / `backend-specialist`
- **Priority:** P0 (CI Quality Gate)
- **Output:**
  1. `tests/unit/laser_system_test.cpp`
  2. `tests/benchmark/laser_benchmark.cpp`
  3. Update `tests/CMakeLists.txt`.

---

## 6. Phase X: Final Verification Checklist

- [ ] `docs/adr/ADR-0021-laser-dac-and-ilda-control.md` written and approved
- [ ] Operator specs for all 4 laser operators created in `docs/operator-spec/`
- [ ] `LaserEngine`, `LaserGeomOp`, `LaserDACChanOp`, `LaserPatternGeomOp`, `LaserFileInGeomOp` implemented
- [ ] Operators registered in `CoreNodes.cpp` and `CMakeLists.txt`
- [ ] `samples/12_laser_symphony_spectacular/laser_symphony_spectacular.nfp` created with documentation
- [ ] `tests/unit/laser_system_test.cpp` and `tests/benchmark/laser_benchmark.cpp` added to CMake
- [ ] 100 kpps DAC streaming benchmark verified with zero memory leaks
- [ ] All automated tests passing (100% pass rate)
- [ ] `STATUS.md` and `.agent/memory/MEMORY.md` updated
