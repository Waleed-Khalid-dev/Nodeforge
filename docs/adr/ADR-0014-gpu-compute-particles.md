# ADR-0014: GPU Compute Particle Simulation System

**Status:** Accepted  
**Date:** 2026-08-21  
**Author:** Waleed Khalid  
**Deciders:** Core Architecture Team  
**Phase:** 15 (Expansion Toward “Almost Everything” — Epic 15.1)  

---

## 1. Context & Problem Statement

Neo Realms' flagship experiences (large-scale architectural façade mapping, interactive hologram installations, and walkable floor plans) require dynamic, organic visual effects such as interactive particle clouds, fluid-like turbulence swarms, and gesture-attracted energy fields.

Simulating hundreds of thousands to millions of particles on the CPU creates severe memory bandwidth bottlenecks and drops below the 60 FPS real-time threshold. NodeForge requires a dedicated, zero-bottleneck GPU Compute Particle System powered by Vulkan 1.3 compute pipelines while maintaining seamless integration with existing `GeomOp`, `ChanOp`, `MatOp`, and `TexOp` pipelines.

---

## 2. Decision & Architecture

### 2.1 Double-Buffered GPU Storage (`ParticleBuffer`)
Particles are stored in double-buffered GPU VMA storage buffers (`VkBuffer` with `VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT`):

```cpp
struct alignas(16) GpuParticle {
    glm::vec4 positionAndLife; // xyz: position, w: remaining life [0.0 .. 1.0]
    glm::vec4 velocityAndMass; // xyz: velocity, w: inverse mass
    glm::vec4 colorAndSize;    // rgba: base color, w: radius/point size
};
```

Double buffering (`Ping` / `Pong`) ensures read/write hazard-free GPU execution where compute shaders read state at frame $t$ and write updated state at frame $t+\Delta t$.

### 2.2 Numerical Integration & Modular Force Accumulation
Physics simulation uses explicit Euler / Verlet integration in a Vulkan compute shader (`particle_sim.comp`):

$$\vec{v}_{t+\Delta t} = (\vec{v}_t + \vec{a}_{\text{total}} \cdot \Delta t) \cdot (1.0 - \mu_{\text{drag}} \cdot \Delta t)$$
$$\vec{p}_{t+\Delta t} = \vec{p}_t + \vec{v}_{t+\Delta t} \cdot \Delta t$$

Forces evaluated in compute passes:
1. **Directional Gravity:** Global acceleration vector $\vec{g}$.
2. **Viscous Drag:** Air resistance proportional to velocity.
3. **3D Simplex Curl Noise Field:** Non-divergent fluid-like turbulence $\nabla \times \vec{\Psi}(\vec{p}, t)$ preserving mass without volume collapse.
4. **Point Attractors / Repulsors:** Inverse-distance gravitational / electrostatic attractors modulated dynamically via OSC/MIDI sensor inputs:
   $$\vec{F}_{\text{attract}} = \frac{S \cdot (\vec{p}_{\text{target}} - \vec{p})}{\max(\|\vec{p}_{\text{target}} - \vec{p}\|^n, \epsilon)}$$
5. **Boundary Damping:** Bounding box and floor-plane inelastic collision restitution.

### 2.3 Interoperability & Rendering Pipeline
- **Emitters (`ParticleEmitterGeomOp`)**: Emit from point origins, geometric volumes, or sampled surface vertices of any upstream `GeomOp` mesh.
- **Modifiers (`ParticleForceGeomOp`, `ParticleAttractorGeomOp`)**: Stackable DAG nodes accumulating force vectors.
- **Rendering (`ParticleMatOp`, `RenderTexOp`)**: Particles convert directly to `GeometryData` vertex streams for point-sprite or camera-facing billboard quad rendering with soft depth fading.

---

## 3. Consequences

### Positive
- **Million-Particle Scale:** Simulates over 1,000,000 active particles at 60+ FPS on RTX 3060+ GPUs.
- **Zero VRAM Leakage:** Fixed-size VMA ring buffers recycle dead particles with zero per-frame allocation.
- **Ecosystem Integration:** Direct parameter binding with `OSCInChanOp`, `AudioFileInChanOp`, and `WarpBlendTexOp`.

### Negative / Mitigations
- **Headless GPU Testing:** Headless unit test environments without full Vulkan hardware fall back to SIMD-accelerated CPU verification in `ParticleBuffer`.
