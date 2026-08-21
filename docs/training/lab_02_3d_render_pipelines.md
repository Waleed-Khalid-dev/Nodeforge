# Workshop Lab 2: 3D Scene Composition & Procedural Geometry

**Module:** 3 | **Objective:** Create a 3D procedural landscape terrain with lighting, deformation, and dynamic rasterization.

---

## 1. Step-by-Step Build Instructions

1. **Generate Terrain Grid:**
   - Press `Tab`, create a `GridGeomOp`.
   - Set `size_x` = `15.0`, `size_y` = `15.0`, `rows` = `40`, `cols` = `40`.
2. **Apply Procedural Noise Deformation:**
   - Create a `NoiseDeformGeomOp` and wire `grid1.output` -> `noisedeform1.input`.
   - Set `frequency` = `0.8`, `amplitude` = `1.8`, `direction` = `Normal`.
   - Set `time` expression: `me.time * 0.5`.
3. **Calculate Smooth Normals:**
   - Create a `NormalsGeomOp` and wire `noisedeform1.output` -> `normals1.input`.
   - Set `mode` = `Smooth`.
4. **Create Lighting Material:**
   - Create a `PhongMatOp`.
   - Set `diffuse` = `[0.2, 0.6, 0.9, 1.0]`, `specular` = `[1.0, 1.0, 1.0, 1.0]`, `shininess` = `64.0`.
5. **Assemble 3D Geometry Component:**
   - Create a `GeometryComp`.
   - Wire `normals1.output` -> `geometry1.geometry` and `phong1.output` -> `geometry1.material`.
6. **Add Camera and Light:**
   - Create a `CameraComp`: set `position` = `[0.0, 8.0, 14.0]`, `target` = `[0.0, 0.0, 0.0]`, `fov` = `60.0`.
   - Create a `LightComp`: set `position` = `[10.0, 15.0, 10.0]`, `intensity` = `1.5`.
7. **Render Scene to Texture:**
   - Create a `RenderTexOp`.
   - Wire `geometry1.output` -> `render1.geometry`, `camera1.output` -> `render1.camera`, `light1.output` -> `render1.light`.
   - Wire `render1.output` to a `ToWindowTexOp` to inspect the procedural 3D terrain!
