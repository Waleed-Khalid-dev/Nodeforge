# Implementation Plan — Phase 15 Epic 15.7: Volumetric Fog & Real-Time GPU Ray Marching Engine

**Phase:** 15 (Expansion Toward “Almost Everything”)  
**Epic:** 15.7 (Volumetric Fog & Real-Time GPU Ray Marching Engine — Cloud/Smoke SOP & VolumetricMatOp)  
**Status:** In Planning  
**Target Milestone:** M7 (Advanced Visual FX & Dynamic Tools)  
**Company:** Neo Realms  
**Owner:** Waleed Khalid  

---

## 1. Overview & Objectives

Epic 15.7 introduces a state-of-the-art **Volumetric Fog & Real-Time GPU Ray Marching Subsystem** to NodeForge. Built for atmospheric clouds, localized smoke plumes, toxic gas zones, volumetric god-rays, and cosmic nebula installations.

Key capabilities:
- **`VolumetricMath` (`src/render/VolumetricMath.h/.cpp`)**: Core mathematical algorithms for 3D Simplex fractal noise, Worley/Cellular distance noise, Beer-Lambert optical extinction ($T = e^{-\sigma_t \rho s}$), and Henyey-Greenstein anisotropic phase scattering ($g \in [-1, 1]$).
- **`VolumetricCloudTexOp` (Family: `TexOp`)**: Full-screen ray-marched atmospheric cloud and sky generator with procedural 3D noise octaves, sun directional lighting, silver-lining forward scattering, and wind drift.
- **`VolumetricFogMatOp` (Family: `MatOp`)**: Localized 3D volume bounding box / mesh material for localized smoke plumes, ground fog, and steam geysers in `Scene3DPass`.
- **`VoxelGridGeomOp` (Family: `GeomOp`)**: 3D voxel density scalar field generator producing volumetric grid bounds and isosurfaces.
- **`LightShaftTexOp` (Family: `TexOp`)**: Post-processing screen-space radial blur volumetric god-ray generator with exposure, decay, and occlusion masking.
- **Cross-Subsystem Synergy**: Volumetric fog tracks `LightComp` sun angles and shadow rays, renders stereoscopically in VR (`OpenXRCameraComp`), and dynamically pulses cloud density and wind swirls to audio frequencies (`AudioSpatializerChanOp`).

---

## 2. Architecture & Volumetric Radiative Transfer Model (ADR-0020)

### 2.1 Beer-Lambert Law & Ray Accumulation
For a camera ray $\vec{r}(t) = \vec{o} + t \cdot \hat{d}$ stepping through volume with step size $\Delta s$:

$$\begin{aligned}
\rho_i &= \text{SampleDensity}(\vec{r}(t_i)) \\
\Delta \tau_i &= \sigma_t \cdot \rho_i \cdot \Delta s \\
T_i &= \prod_{k=0}^{i-1} \exp(-\Delta \tau_k) \\
L_{\text{scatter}} &= L_{\text{sun}} \cdot p(\theta, g) \cdot \exp(-\tau_{\text{light}}) \\
C_{\text{final}} &= \sum_{i} T_i \cdot (1 - \exp(-\Delta \tau_i)) \cdot L_{\text{scatter}}
\end{aligned}$$

### 2.2 Henyey-Greenstein Anisotropic Phase Function
For angle $\theta$ between view ray and sun light direction ($\cos\theta = \hat{d} \cdot \hat{L}$) and asymmetry parameter $g \in (-1, 1)$:

$$p(\theta, g) = \frac{1 - g^2}{4\pi (1 + g^2 - 2g \cos\theta)^{3/2}}$$

---

## 3. Operator Suite & Specifications

### 1. `VolumetricCloudTexOp` (Family: `TexOp`)
- **Input Pins:** None (generates procedural volume) or optional `in_env` (Tex).
- **Output Pins:** `output` (Tex).
- **Parameters:**
  - `density`: `float` (default: `1.0`, range: `0.0 .. 10.0`).
  - `sun_dir`: `vec3` (default: `[0.5, 0.8, -0.3]`).
  - `sun_color`: `vec4` (default: `[1.0, 0.9, 0.7, 1.0]`).
  - `scattering_g`: `float` (default: `0.6`, range: `-0.9 .. 0.9`).
  - `step_count`: `int` (default: `32`, range: `8 .. 128`).
  - `wind_speed`: `float` (default: `0.5`).

### 2. `VolumetricFogMatOp` (Family: `MatOp`)
- **Input Pins:** None.
- **Output Pins:** `output` (Mat).
- **Parameters:**
  - `density_scale`: `float` (default: `2.0`).
  - `fog_color`: `vec4` (default: `[0.7, 0.8, 0.9, 1.0]`).
  - `noise_frequency`: `float` (default: `1.5`).
  - `absorption`: `float` (default: `0.5`).

### 3. `VoxelGridGeomOp` (Family: `GeomOp`)
- **Input Pins:** None.
- **Output Pins:** `output` (Geom).
- **Parameters:**
  - `grid_resolution`: `int` (default: `32`, range: `8 .. 128`).
  - `box_size`: `vec3` (default: `[10.0, 10.0, 10.0]`).
  - `field_type`: `0: Sphere Isosurface`, `1: Noise Fractal`, `2: Torus SDF`.

### 4. `LightShaftTexOp` (Family: `TexOp`)
- **Input Pins:** `input` (Tex), `in_occlusion` (Tex, optional).
- **Output Pins:** `output` (Tex).
- **Parameters:**
  - `light_pos_screen`: `vec2` (default: `[0.5, 0.5]`).
  - `density`: `float` (default: `0.8`).
  - `decay`: `float` (default: `0.95`, range: `0.8 .. 1.0`).
  - `weight`: `float` (default: `0.5`).
  - `samples`: `int` (default: `32`, range: `8 .. 64`).

---

## 4. Planned File Structure

```
d:\[Project]\Touch Designer\
├── docs\
│   ├── adr\
│   │   └── ADR-0020-volumetric-fog-and-ray-marching.md
│   ├── operator-spec\
│   │   ├── VolumetricCloudTexOp.md
│   │   ├── VolumetricFogMatOp.md
│   │   ├── VoxelGridGeomOp.md
│   │   └── LightShaftTexOp.md
│   └── cheat-sheets\
│       └── 13_volumetric_cheat_sheet.md
├── src\
│   ├── render\
│   │   └── VolumetricMath.h/.cpp        # Beer-Lambert, Henyey-Greenstein, Simplex/Worley noise
│   └── operators\
│       ├── tex\
│       │   ├── VolumetricCloudTexOp.h/.cpp
│       │   └── LightShaftTexOp.h/.cpp
│       ├── mat\
│       │   └── VolumetricFogMatOp.h/.cpp
│       └── geom\
│           └── VoxelGridGeomOp.h/.cpp
├── samples\
│   └── 11_volumetric_nebula_cathedral\
│       ├── volumetric_nebula_cathedral.nfp # Flagship volumetric project
│       └── README.md
└── tests\
    ├── unit\
    │   └── volumetric_system_test.cpp      # Unit tests for Beer-Lambert, phase, noise, voxels
    └── benchmark\
        └── volumetric_benchmark.cpp        # 60 FPS ray marching throughput benchmark
```

---

## 5. Detailed Task Breakdown

### Task 15.7.1: Architecture Documentation & Operator Specifications
- **Agent:** `project-planner` / `architecture`
- **Priority:** P0 (Design Pre-requisite)
- **Output:**
  1. `docs/adr/ADR-0020-volumetric-fog-and-ray-marching.md`
  2. `docs/operator-spec/VolumetricCloudTexOp.md`
  3. `docs/operator-spec/VolumetricFogMatOp.md`
  4. `docs/operator-spec/VoxelGridGeomOp.md`
  5. `docs/operator-spec/LightShaftTexOp.md`
  6. `docs/cheat-sheets/13_volumetric_cheat_sheet.md`

### Task 15.7.2: Volumetric Engine & Operator Implementations
- **Agent:** `backend-specialist` / `game-developer`
- **Priority:** P0 (Core Implementation)
- **Output:**
  1. `src/render/VolumetricMath.h/.cpp`
  2. `src/operators/tex/VolumetricCloudTexOp.h/.cpp`
  3. `src/operators/tex/LightShaftTexOp.h/.cpp`
  4. `src/operators/mat/VolumetricFogMatOp.h/.cpp`
  5. `src/operators/geom/VoxelGridGeomOp.h/.cpp`
  6. Register all operators in `src/graph/CoreNodes.cpp` and `CMakeLists.txt`.

### Task 15.7.3: Showcase Sample Project (`samples/11_volumetric_nebula_cathedral/`)
- **Agent:** `project-planner` / `backend-specialist`
- **Priority:** P1 (Production Sample)
- **Output:**
  1. `samples/11_volumetric_nebula_cathedral/volumetric_nebula_cathedral.nfp`
  2. `samples/11_volumetric_nebula_cathedral/README.md`

### Task 15.7.4: Automated Unit Tests & 60 FPS Benchmark
- **Agent:** `test-engineer` / `backend-specialist`
- **Priority:** P0 (CI Quality Gate)
- **Output:**
  1. `tests/unit/volumetric_system_test.cpp`
  2. `tests/benchmark/volumetric_benchmark.cpp`
  3. Update `tests/CMakeLists.txt`.

---

## 6. Phase X: Final Verification Checklist

- [ ] `docs/adr/ADR-0020-volumetric-fog-and-ray-marching.md` written and approved
- [ ] Operator specs for all 4 volumetric operators created in `docs/operator-spec/`
- [ ] `VolumetricMath`, `VolumetricCloudTexOp`, `VolumetricFogMatOp`, `VoxelGridGeomOp`, `LightShaftTexOp` implemented
- [ ] Operators registered in `CoreNodes.cpp` and `CMakeLists.txt`
- [ ] `samples/11_volumetric_nebula_cathedral/volumetric_nebula_cathedral.nfp` created with documentation
- [ ] `tests/unit/volumetric_system_test.cpp` and `tests/benchmark/volumetric_benchmark.cpp` added to CMake
- [ ] 60 FPS ray marching benchmark verified with zero memory leaks
- [ ] All automated tests passing (100% pass rate)
- [ ] `STATUS.md` and `.agent/memory/MEMORY.md` updated
