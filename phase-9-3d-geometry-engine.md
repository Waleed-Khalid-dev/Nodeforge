# Phase 9: 3D Geometry, Materials & Render Pipeline Plan

**Slug:** `phase-9-3d-geometry-engine.md`  
**Phase:** 9 — Geometry + Materials + Render (3D Path / GeomOps / SOP)  
**Status:** APPROVED & READY FOR IMPLEMENTATION (`/create`)  
**Architecture Record:** `docs/adr/ADR-0009-geometry-materials-and-render-pipeline.md`

---

## 1. Executive Summary & Goals

Phase 9 establishes the complete 3D generative synthesis and rasterization engine for NodeForge. It enables procedural 3D mesh creation, geometry deformations, lighting, custom material shaders, and high-density GPU particle instancing, outputting to high-resolution Vulkan 2D textures that seamlessly feed into the existing TexOp compositor.

### Key Targets:
- **Core 3D Data Model:** `GeometryData` (interleaved `Vertex` struct with pos, norm, uv, color, tangent, 32-bit indices, bounding boxes, auto-normals).
- **Vulkan 1.3 3D Rasterizer (`RenderTexOp`):** Dynamic rendering pass with `D32_SFLOAT` depth buffer, camera view/projection, Blinn-Phong & PBR shaders, multi-light support, and MSAA.
- **15-Node Operator Suite:**
  - 5 Primitives: `GridGeomOp`, `SphereGeomOp`, `BoxGeomOp`, `TorusGeomOp`, `CylinderGeomOp`.
  - 4 Modifiers: `TransformGeomOp`, `MergeGeomOp`, `NoiseDeformGeomOp`, `NormalsGeomOp`.
  - 1 Interop: `ChanToGeomOp` (ChopToSop).
  - 3 Materials: `ConstantMatOp`, `PhongMatOp`, `GLSLMatOp` (runtime shader compiler).
  - 2 Scene Comps: `CameraComp`, `LightComp` + `GeometryComp`.
- **GPU Instancing:** Dual-source ingestion from `ChannelBuffer` and `DataTable` supporting 100,000+ instances at 60+ FPS.
- **Interactive UI 3D Viewport:** Arcball/Turntable camera navigation, grid floor plane, shading modes (Lit, Flat, Wireframe, Normals), and canvas mini-wireframes.
- **Verification:** 100% test pass rate on full test suite (75+ tests) with zero GPU memory leaks across 10,000 frames.

---

## 2. Technical Breakdown & Tasks

### Subphase 9.1 — Core 3D Data Structure (`src/core/GeometryData.*`)
- `Vertex` layout: `glm::vec3 pos`, `glm::vec3 normal`, `glm::vec2 uv`, `glm::vec4 color`, `glm::vec4 tangent`.
- `GeometryData`:
  - `std::vector<Vertex> m_vertices`, `std::vector<uint32_t> m_indices`.
  - Primitive generators: `CreateGrid`, `CreateSphere`, `CreateBox`, `CreateTorus`, `CreateCylinder`.
  - Mesh operations: `Transform(mat4)`, `Merge(other)`, `ComputeNormals(smooth)`, `ComputeTangents()`, `ComputeBounds()`, `DeformNoise(freq, amp, offset)`.
- Update `PinValue.h` & `PinValue.cpp` to add `GeometryData` to `VariantType`.

### Subphase 9.2 — Base Classes (`GeomOp`, `MatOp`, `Comp`)
- `src/operators/geom/GeomOp.h` & `GeomOp.cpp`: Base class for geometry nodes.
- `src/operators/mat/MatOp.h` & `MatOp.cpp`: Base class for material operators (ambient, diffuse, specular, shaders).
- `src/operators/comp/CameraComp.h` & `CameraComp.cpp`: Camera component (FOV, aspect, near/far, view/projection matrix generation, orbit).
- `src/operators/comp/LightComp.h` & `LightComp.cpp`: Light component (Directional, Point, Ambient, color, intensity).
- `src/operators/comp/GeometryComp.h` & `GeometryComp.cpp`: Geometry holder with material link, transform, and instancing buffer.

### Subphase 9.3 — 3D Operator Suite Implementations
- **Primitives:** `GridGeomOp`, `SphereGeomOp`, `BoxGeomOp`, `TorusGeomOp`, `CylinderGeomOp`.
- **Modifiers:** `TransformGeomOp`, `MergeGeomOp`, `NoiseDeformGeomOp`, `NormalsGeomOp`.
- **Interop:** `ChanToGeomOp` (converts `ChannelBuffer` into 3D points / instance grids).
- **Materials:** `ConstantMatOp`, `PhongMatOp`, `GLSLMatOp` (custom GLSL vertex/fragment shaders with hot-reloading).

### Subphase 9.4 — Vulkan 3D Dynamic Render Pipeline (`RenderTexOp`)
- `src/render/Scene3DPass.h` & `Scene3DPass.cpp`:
  - Allocates / leases `D32_SFLOAT` depth texture from `TexturePool`.
  - Binds Vulkan 1.3 dynamic rendering color and depth attachments.
  - Binds 3D pipeline (Blinn-Phong / PBR shader, depth test enabled, depth write enabled, backface culling).
  - Iterates over scene `GeometryComp` objects, binds VBO/IBO, pushes model/view/proj matrices, and issues `vkCmdDrawIndexed` / instanced draws.
- `src/operators/tex/RenderTexOp.h` & `RenderTexOp.cpp`:
  - TexOp rendering 3D scene graph into `gpu::Texture2D`.

### Subphase 9.5 — Registry, Python API & Editor UI Viewport
- Register all 15 3D node types in `src/graph/CoreNodes.cpp`.
- Expose `GeometryData`, `node.geometry`, `Vertex` to Python in `src/python/PyNodeForge.cpp`.
- Upgrade `src/ui/panels/ViewerPanel.cpp` with interactive 3D Arcball / Turntable viewport, grid floor, wireframe overlay, and camera controls.
- Update `src/ui/canvas/NodeCanvas.cpp` with mini isometric 3D wireframe preview on GeomOp nodes.

### Subphase 9.6 — Build, Test & Benchmark
- Update `CMakeLists.txt` and `tests/CMakeLists.txt`.
- Author `tests/unit/geometry_data_test.cpp`, `tests/unit/geomop_pipeline_test.cpp`, and `tests/benchmark/geomop_benchmark.cpp`.
- Compile with MSVC `vcvars64.bat` and run `nodeforge_tests.exe` (aiming for all 75+ tests passing).

---

## 3. Operator Specification Files (`docs/operator-spec/`)
Before code implementation, the following 15 specs will be written:
1. `GridGeomOp.md`
2. `SphereGeomOp.md`
3. `BoxGeomOp.md`
4. `TorusGeomOp.md`
5. `CylinderGeomOp.md`
6. `TransformGeomOp.md`
7. `MergeGeomOp.md`
8. `NoiseDeformGeomOp.md`
9. `NormalsGeomOp.md`
10. `ChanToGeomOp.md`
11. `ConstantMatOp.md`
12. `PhongMatOp.md`
13. `GLSLMatOp.md`
14. `CameraComp.md`
15. `LightComp.md`

---

## 4. Definition of Done (DoD)
- [ ] 15 operator specs present in `docs/operator-spec/`.
- [ ] `GeometryData` CPU mesh data structure with vertex/index buffers and auto-normals passing unit tests.
- [ ] `RenderTexOp` renders a lit 3D sphere / grid with camera and lights into a Vulkan 2D texture.
- [ ] Custom `GLSLMatOp` material hot-reloads vertex and fragment shaders without restart.
- [ ] GPU instancing renders 100,000+ instances from `ChannelBuffer` / `DataTable` at >60 FPS.
- [ ] Interactive 3D Arcball camera navigation working in `ViewerPanel`.
- [ ] Full automated test suite passes (75+ tests, 100% pass rate).
- [ ] `STATUS.md` and `02-BUILD-ROADMAP-A-to-Z.md` updated and signed off.
