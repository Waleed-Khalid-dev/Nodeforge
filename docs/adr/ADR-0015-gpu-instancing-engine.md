# ADR-0015: Advanced GPU Instancing Engine & Dynamic Attribute Distribution

**Status:** Accepted  
**Date:** 2026-08-21  
**Author:** Waleed Khalid  
**Deciders:** Core Architecture Team  
**Phase:** 15 (Expansion Toward “Almost Everything” — Epic 15.2)  

---

## 1. Context & Problem Statement

Large-scale multimedia installations, holographic matrices, and architectural projection shows require rendering thousands to hundreds of thousands of duplicate geometric objects (crystals, light fixtures, architectural panels, kinetic sculpture components).

Duplicating geometry meshes on the CPU creates enormous memory bandwidth bottlenecks and vertex transform overhead. NodeForge requires a dedicated GPU Instancing Engine capable of drawing over 100,000 instances in a single draw call with per-instance transformations, color ramps, audio modulation, and procedural distribution topologies (tables, channels, surface normals, Fibonacci spirals).

---

## 2. Decision & Architecture

### 2.1 GPU Instance Buffer Layout
Instance attributes are stored in contiguous 64-byte aligned structs in `GeometryData::m_instances` and uploaded to Vulkan secondary vertex buffers (`VkBuffer` with `VK_BUFFER_USAGE_VERTEX_BUFFER_BIT` binding 1, `VK_VERTEX_INPUT_RATE_INSTANCE`):

```cpp
struct alignas(16) InstanceData {
    glm::mat4 transform{1.0f};      // 64 bytes: 4x4 Affine World Matrix
    glm::vec4 color{1.0f};          // 16 bytes: RGBA color multiplier
    glm::vec4 customParams{0.0f};   // 16 bytes: x: UV atlas offset, y: audio scale, z: time phase, w: flags
};
```

### 2.2 Dedicated `InstanceGeomOp` Operator
`InstanceGeomOp` operates on a base template geometry mesh (e.g. Box, Sphere, Torus, or custom mesh) and generates an instanced geometry output across 4 distribution topologies:

1. **Table & Channel 1-to-1 Mapping:** Directly extracts coordinates and attributes from `DataTable` rows or `ChannelBuffer` streams.
2. **Mesh Surface & Normal Alignment:** Instantiates a copy at each vertex of a distribution mesh, aligning local orientation to the vertex normal $\vec{n}$ using quaternion rotation:
   $$\mathbf{q} = \text{RotationBetween}(\vec{u}_{\text{up}}, \vec{n})$$
3. **Parametric Grid Array:** Generates regular 1D/2D/3D grids with dimensions $(N_x, N_y, N_z)$, spacing, and 3D Simplex noise jitter displacement.
4. **Fibonacci Phyllotaxis Spiral:** Generates golden ratio spiral layouts ($\theta = n \cdot 137.5077^{\circ}$, $r = c \sqrt{n}$) with progressive scale attenuation and angular twist.

### 2.3 Multi-Attribute Modulation
- **Color Gradients:** Ramped by instance index ($i/N$), distance from origin ($d / r_{\text{max}}$), or external channel values.
- **Dynamic Wave Ripples:** Direct modulation from `AudioFileInChanOp` spectrum bands or `TimeChanOp` for kinetic wave motion.

---

## 3. Consequences

### Positive
- **100,000+ Instances at 60 FPS:** Single-pass instanced rendering via Vulkan 1.3 `vkCmdDrawIndexed` with `instanceCount > 100,000`.
- **Zero VRAM Leakage:** Fixed-size instance buffers reuse memory without per-frame allocations.
- **Deep Family Interop:** Seamlessly binds to `DataTable`, `ChannelBuffer`, and downstream `GeometryComp` / `RenderTexOp`.

### Negative / Mitigations
- **Large Array Memory:** 100,000 instances require ~9.6 MB of instance buffer memory; managed cleanly in `GeometryData`.
