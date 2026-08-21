# ADR-0020: Volumetric Fog & Real-Time GPU Ray Marching Engine

**Status:** Accepted  
**Date:** 2026-08-21  
**Author:** Waleed Khalid  
**Deciders:** Core Architecture Team  
**Phase:** 15 (Expansion Toward “Almost Everything” — Epic 15.7)  

---

## 1. Context & Problem Statement

Atmospheric realism in live concert visuals, projection mapping on smoke/water mist screens, architectural cathedral light shafts, and sci-fi cosmic nebulas require physically-based 3D volumetric light scattering and real-time GPU ray marching.

Flat 2D particle billboards cannot reproduce continuous density fields, volumetric shadows cast by occluders, or forward Mie/Henyey-Greenstein silver-lining solar highlights. NodeForge requires a native, GPU-accelerated **Volumetric Radiative Transfer & Ray Marching Engine** supporting procedural 3D noise fields, localized mesh volumes, and screen-space light shafts.

---

## 2. Decision & Architecture

### 2.1 Beer-Lambert Optical Transmittance
For light ray $\vec{r}(t) = \vec{o} + t \cdot \hat{d}$ through a participating medium with extinction coefficient $\sigma_t$ and density field $\rho(\vec{x})$:

$$T(s) = \exp\left( -\sigma_t \int_0^s \rho(\vec{r}(t)) \, dt \right)$$

### 2.2 Henyey-Greenstein Anisotropic Phase Function
For angle $\theta$ between view direction and incident sun light ($\cos\theta = \hat{d} \cdot \hat{L}$):

$$p(\theta, g) = \frac{1 - g^2}{4\pi (1 + g^2 - 2g \cos\theta)^{3/2}}$$

where $g \in (-1, 1)$ governs forward ($g > 0$) or backward ($g < 0$) scattering.

### 2.3 Modular Operator Suite
- `VolumetricCloudTexOp` (Family: `TexOp`): Full-screen procedural 3D ray-marched atmospheric clouds and sky generator.
- `VolumetricFogMatOp` (Family: `MatOp`): 3D volume material for localized smoke plumes and fog zones in `Scene3DPass`.
- `VoxelGridGeomOp` (Family: `GeomOp`): Generates 3D voxel density scalar fields and isosurfaces.
- `LightShaftTexOp` (Family: `TexOp`): Post-processing screen-space radial blur volumetric god-ray generator.

---

## 3. Consequences

### Positive
- **Photorealistic Atmosphere:** Authentic light extinction and forward silver-lining highlights.
- **Cross-Subsystem Synergy:** Scene lights dynamically cast volumetric shadows; audio frequencies modulate cloud density and wind swirls.
- **High Performance:** Sub-millisecond pipeline cook at 60–120 FPS.
