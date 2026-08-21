# Implementation Plan — Phase 15 Epic 15.2: Advanced GPU Instancing Engine & Dynamic Attribute Distribution

**Phase:** 15 (Expansion Toward “Almost Everything”)  
**Epic:** 15.2 (Advanced GPU Instancing & Kinetic Multi-Mesh Matrix)  
**Status:** In Planning  
**Target Milestone:** M7 (Advanced Visual FX & Dynamic Simulations)  
**Company:** Neo Realms  
**Owner:** Waleed Khalid  

---

## 1. Overview & Objectives

Epic 15.2 delivers an enterprise-grade, high-performance **GPU Instancing Engine** to NodeForge. Built to render hundreds of thousands of dynamic 3D meshes (cubes, crystals, spheres, architectural elements) at 60+ FPS, this system empowers Neo Realms to craft massive kinetic matrix installations, procedural generative architectural facades, and audio-reactive 3D fields.

Key capabilities include:
- **`InstanceGeomOp`**: Multi-mode procedural instancing generator and transformer.
- **Multi-Source Data Binding**: Seamlessly drive instance positions, rotations, scales, colors, and UV atlas offsets from:
  - **DataOp Tables**: CSV spreadsheets, JSON schemas, live runtime coordinate lists.
  - **ChanOp SIMD Streams**: High-frequency audio spectrum channels, LFO oscillators, and OSC LiDAR tracking.
  - **GeomOp Surface Distribution**: Scatter instances across surface vertices with automatic surface normal $\vec{N}$ orientation alignment.
  - **Procedural Geometric Layouts**: 1D/2D/3D matrix grids, Fibonacci phyllotaxis spirals, and parametric curves.
- **Vulkan 1.3 Secondary Vertex Buffers**: High-throughput rendering through interleaved instance buffers (`VkBuffer` binding 1: `mat4 transformMatrix`, `vec4 color`, `vec4 customParams`) supporting over 100,000 instances with zero per-frame allocation overhead.

---

## 2. Architecture & Data Structures (ADR-0015)

### 2.1 GPU Instance Layout
Instance data is organized into cache-coherent, 64-byte aligned structs suitable for direct GPU upload or shader storage buffer object (SSBO) binding:

```cpp
struct alignas(16) GpuInstanceData {
    glm::mat4 transform{1.0f};      // 64 bytes: 4x4 Affine World Matrix (Translation, Rotation, Scale)
    glm::vec4 color{1.0f};          // 16 bytes: RGBA color multiplier / tint
    glm::vec4 customParams{0.0f};   // 16 bytes: x: UV atlas index / time offset, y: audio reactive scale, z: depth offset, w: flags
};
```

### 2.2 Procedural Distribution Modes
`InstanceGeomOp` supports 4 fundamental distribution topologies:
1. **Direct Table / Channel Mode (`Mode: 0`)**: 1-to-1 mapping from columns/channels (`tx, ty, tz, rx, ry, rz, sx, sy, sz, r, g, b, a`).
2. **Mesh Surface & Normal Alignment (`Mode: 1`)**: Places an instance on each vertex of a distribution mesh, aligning the local +Y or +Z axis to the surface vertex normal $\vec{n}$ using quaternion rotation:
   $$\mathbf{q} = \text{RotationBetween}(\vec{u}_{\text{up}}, \vec{n})$$
3. **Parametric Grid / Volume Array (`Mode: 2`)**: Regular 1D/2D/3D grids with dimensions $(N_x, N_y, N_z)$, configurable spacing, center origin, and 3D Simplex jitter displacement.
4. **Fibonacci Spiral / Kinetic Phyllotaxis (`Mode: 3`)**: Organic sunflower / spiral distribution with golden ratio angle $\theta = n \cdot 137.5077^{\circ}$ and radius $r = c \sqrt{n}$, progressive scale attenuation, and rotational twist.

---

## 3. Operator Suite & Specifications

### `InstanceGeomOp` (Family: `GeomOp`)
- **Input Pins:**
  - `template_mesh` (Geom, Required): Base 3D geometry mesh (e.g. Box, Sphere, Torus, custom model) to be duplicated.
  - `distribution_mesh` (Geom, Optional): Target mesh whose vertices/normals define instance positions and orientations.
  - `instances_data` (Data, Optional): 2D table containing instance transformation rows.
  - `instances_chan` (Chan, Optional): SIMD channels modulating instance transforms or colors.
- **Output Pins:**
  - `output` (Geom): Complete instanced geometry containing template vertices + populated `InstanceData` array.
- **Parameters:**
  - `distribution_mode`: `0: Table/Channel`, `1: Mesh Surface`, `2: Grid Array`, `3: Fibonacci Spiral`.
  - `grid_count`: `ivec3` array dimensions (default: `[30, 30, 1]`, up to `100000`).
  - `grid_spacing`: `vec3` spacing distance (default: `[1.0, 1.0, 1.0]`).
  - `spiral_count`: `int` number of spiral nodes (default: `5000`).
  - `spiral_radius_scale`: `float` growth factor (default: `0.5`).
  - `spiral_twist`: `float` rotation angle progression in degrees (default: `137.5`).
  - `noise_displacement`: `float` 3D Simplex noise jitter amplitude (default: `0.0`).
  - `noise_frequency`: `float` spatial frequency of transform noise (default: `0.5`).
  - `align_to_normals`: `bool` orient instances to surface normals (default: `true`).
  - `color_mode`: `0: Uniform`, `1: Index Ramp`, `2: Distance Ramp`, `3: Table/Channel`.
  - `start_color`: `vec4` (default: `[0.1, 0.7, 1.0, 1.0]`).
  - `end_color`: `vec4` (default: `[1.0, 0.2, 0.6, 1.0]`).
  - `audio_scale_mod`: `float` amplitude scale driven by incoming audio channels (default: `1.0`).

---

## 4. Planned File Structure

```
d:\[Project]\Touch Designer\
├── docs\
│   ├── adr\
│   │   └── ADR-0015-gpu-instancing-engine.md
│   ├── operator-spec\
│   │   └── InstanceGeomOp.md
│   └── cheat-sheets\
│       └── 08_instancing_cheat_sheet.md
├── src\
│   └── operators\
│       └── geom\
│           ├── InstanceGeomOp.h
│           └── InstanceGeomOp.cpp
├── samples\
│   └── 06_matrix_instancing\
│       ├── matrix_instancing.nfp       # 100,000-object audio-reactive kinetic matrix
│       └── README.md
└── tests\
    ├── unit\
    │   └── instancing_system_test.cpp  # Unit tests for modes, normal alignment, and serialization
    └── benchmark\
        └── instancing_benchmark.cpp    # 100,000-instance <2ms throughput stress benchmark
```

---

## 5. Detailed Task Breakdown

### Task 15.2.1: Architecture Documentation & Operator Specifications
- **Agent:** `project-planner` / `architecture`
- **Priority:** P0 (Design Pre-requisite)
- **Output:**
  1. `docs/adr/ADR-0015-gpu-instancing-engine.md`
  2. `docs/operator-spec/InstanceGeomOp.md`
  3. `docs/cheat-sheets/08_instancing_cheat_sheet.md`

### Task 15.2.2: `InstanceGeomOp` Operator Implementation
- **Agent:** `backend-specialist`
- **Priority:** P0 (Core Operator)
- **Output:**
  1. `src/operators/geom/InstanceGeomOp.h` & `InstanceGeomOp.cpp`
  2. Register `InstanceGeomOp` in `src/graph/CoreNodes.cpp` and `CMakeLists.txt`.
  3. Multi-mode procedural distribution (Table, Mesh Normal, Grid, Fibonacci Spiral).

### Task 15.2.3: Showcase Sample Project (`samples/06_matrix_instancing/`)
- **Agent:** `project-planner` / `backend-specialist`
- **Priority:** P1 (Showcase Production Sample)
- **Output:**
  1. `samples/06_matrix_instancing/matrix_instancing.nfp` (100,000-crystal audio-reactive kinetic matrix)
  2. `samples/06_matrix_instancing/README.md`

### Task 15.2.4: Automated Unit Tests & 100k Instance Stress Benchmark
- **Agent:** `test-engineer` / `backend-specialist`
- **Priority:** P0 (CI Quality Gate)
- **Output:**
  1. `tests/unit/instancing_system_test.cpp` (Table binding, normal alignment, spiral calculation, JSON roundtrip)
  2. `tests/benchmark/instancing_benchmark.cpp` (100,000-instance evaluation throughput < 2.0 ms)
  3. Register targets in `tests/CMakeLists.txt`.

---

## 6. Phase X: Final Verification Checklist

- [ ] `docs/adr/ADR-0015-gpu-instancing-engine.md` written and approved
- [ ] `docs/operator-spec/InstanceGeomOp.md` created
- [ ] `src/operators/geom/InstanceGeomOp.h/.cpp` implemented and registered
- [ ] `samples/06_matrix_instancing/matrix_instancing.nfp` created with documentation
- [ ] `tests/unit/instancing_system_test.cpp` and `tests/benchmark/instancing_benchmark.cpp` added to CMake
- [ ] 100,000 instances evaluated under 2.0 ms per frame with zero memory leaks
- [ ] All automated tests passing (100% pass rate)
- [ ] `STATUS.md` and `.agent/memory/MEMORY.md` updated
