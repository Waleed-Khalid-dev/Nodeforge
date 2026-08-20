# ADR-0009: 3D Geometry, Materials & Vulkan Render Pipeline

## Status
Accepted (2026-08-20)

## Context
NodeForge requires a complete 3D geometry generation, modification, material shading, and rasterization subsystem (GeomOps/SOPs, MatOps, Scene Comps, and RenderTexOp). This subsystem must seamlessly interoperate with the existing TexOp (GPU textures), ChanOp (SIMD channel buffers), and DataOp (tabular spreadsheets) pipelines, enabling real-time generative 3D visuals, high-density particle instancing, custom GLSL shader authoring, and multi-light rendering for interactive projection mapping and spatial computing.

## Decision

1. **Geometry Data Model (`GeometryData` / `MeshHandle`)**:
   - Flat interleaved `Vertex` struct (32-bit floats: `Position vec3`, `Normal vec3`, `UV vec2`, `Color vec4`, `Tangent vec4`).
   - 32-bit indexed triangle buffers (`std::vector<uint32_t>`).
   - Built-in CPU utility functions: automatic smooth/flat normal generation, tangent calculation, bounding box computation, noise deformation, and vertex transformations.
   - VMA-backed GPU vertex and index buffers (`gpu::Buffer`) uploaded on demand with dirty caching.

2. **Vulkan 1.3 Dynamic Rendering 3D Scene Pass (`RenderTexOp`)**:
   - Uses Vulkan 1.3 Dynamic Rendering (`vkCmdBeginRendering`) without legacy render passes or framebuffers.
   - Reusable `D32_SFLOAT` depth-stencil buffer pool managed alongside `TexturePool`.
   - Push Constants / Uniform Buffers for Camera View/Projection matrices, Model matrices, and Light structs.
   - Multi-light forward shading: Ambient, Directional, and Point light sources with Blinn-Phong and PBR material shaders.
   - Depth test, depth write, and alpha blending modes (Opaque, Additive, Alpha Blend).

3. **Material System (`MatOp`)**:
   - `ConstantMatOp`: Unlit flat color, vertex color tint, and wireframe mode.
   - `PhongMatOp`: Diffuse, specular, ambient, shininess, emissive, and texture map inputs.
   - `GLSLMatOp`: Custom GLSL vertex & fragment shaders with runtime hot-reloading via `gpu::ShaderCompiler`.

4. **Scene Graph Components (`Comp`)**:
   - `GeometryComp`: Encapsulates mesh input, material reference, local transform (Translate, Rotate, Scale), and instancing bindings.
   - `CameraComp`: Perspective (FOV, Aspect, Near, Far) and Orthographic projection modes with Look-At target and orbit controls.
   - `LightComp`: Directional, Point, and Ambient light parameters (Color, Intensity, Attenuation).

5. **High-Performance Instanced Rendering**:
   - Hardware instancing via `vkCmdDrawIndexed(..., instanceCount, ...)` with GPU instance buffer (`vec3 Position`, `vec4 Rotation/Scale`, `vec4 Color`).
   - Dual-source ingestion: Direct instancing from `ChannelBuffer` (ChanOps) and `DataTable` (DataOps), rendering 100,000+ instances at 60+ FPS.

6. **Interactive 3D Viewport & UI**:
   - `ViewerPanel` provides interactive Arcball/Turntable camera orbit (Alt + Left drag = Orbit, Alt + Middle drag = Pan, Scroll = Zoom), grid floor plane, shading mode toggles (Lit, Wireframe, Normals), and camera info overlays.
   - `NodeCanvas` renders mini isometric 3D wireframe bounding previews on GeomOp node bodies.

## Consequences
- Full 3D generative synthesis and geometry manipulation directly in NodeForge.
- Seamless bridge between ChanOps/DataOps and 3D rendering (particle systems, audio-reactive meshes, data-driven geometries).
- Clean foundation for Phase 10b Projection Mapping (UV warping, multi-projector calibration).
