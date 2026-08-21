# ADR-0017: Advanced Spatial Audio Simulation & Multi-Speaker Ambisonics Engine

**Status:** Accepted  
**Date:** 2026-08-21  
**Author:** Waleed Khalid  
**Deciders:** Core Architecture Team  
**Phase:** 15 (Expansion Toward “Almost Everything” — Epic 15.4)  

---

## 1. Context & Problem Statement

Immersive multimedia installations, planetarium fulldome theatres, architectural projection mapping environments, and VR exhibitions require precise, real-time 3D spatialized sound. 

NodeForge needs a native spatial audio engine integrated into its `ChanOp` and `Comp` pipelines capable of:
1. Panning monophonic and stereo audio sources across arbitrary multi-speaker layouts (Stereo, Quad 4.0, 5.1 Surround, 7.1.4 Dolby Atmos ceiling setups, 16-channel linear/dome arrays) using Vector Base Amplitude Panning (VBAP).
2. Encoding sound sources into First-Order Ambisonics ($W, X, Y, Z$) B-format streams and decoding them onto physical loudspeaker topologies.
3. Calculating real-time physical acoustic propagation: inverse-square distance attenuation, directional cone directivity, high-frequency air absorption, and Doppler pitch shifting.
4. Binding sound emitters and virtual listener microphones directly to 3D scene objects and camera components.

---

## 2. Decision & Architecture

### 2.1 Ambisonics B-Format Encoding Formulas
For an audio source $S(t)$ positioned at azimuth $\theta \in [-\pi, \pi]$ and elevation $\phi \in [-\pi/2, \pi/2]$ relative to the listener:

$$\begin{aligned}
W(t) &= \frac{S(t)}{\sqrt{2}} \quad \text{(Zero-Order Omnidirectional)} \\
X(t) &= S(t) \cdot \cos\theta \cdot \cos\phi \quad \text{(First-Order Front-Back)} \\
Y(t) &= S(t) \cdot \sin\theta \cdot \cos\phi \quad \text{(First-Order Left-Right)} \\
Z(t) &= S(t) \cdot \sin\phi \quad \text{(First-Order Up-Down)}
\end{aligned}$$

### 2.2 Vector Base Amplitude Panning (VBAP 3D) & Discrete Decoding
Physical speaker layouts are defined by spherical coordinate vectors $\vec{v}_i = (\theta_i, \phi_i)$.

For standard 5.1 / 7.1.4 layouts:
- Front Left (`L`), Front Right (`R`), Center (`C`), LFE/Sub (`LFE`), Surround Left (`Ls`), Surround Right (`Rs`), Top Front Left (`Tfl`), Top Front Right (`Tfr`), Top Rear Left (`Trl`), Top Rear Right (`Trr`).

For each sound source at vector $\vec{p} = \vec{P}_{\text{emitter}} - \vec{P}_{\text{listener}}$, gains $g_i$ are computed via normalized projection and distance attenuation:

$$A(d) = \frac{1.0}{1.0 + \alpha \cdot d} \quad \text{where } d = \|\vec{p}\|$$

### 2.3 Doppler Pitch Shifting
Relative radial velocity $v_r = (\vec{v}_{\text{emitter}} - \vec{v}_{\text{listener}}) \cdot \hat{p}$ modulates playback rate:

$$f_{\text{observed}} = f_0 \cdot \left( \frac{c}{c - v_r} \right)$$

where $c = 343.0\text{ m/s}$ (speed of sound in air).

---

## 3. Consequences

### Positive
- **Hardware Agnostic:** Easily scales from 2-speaker headphones to 64-speaker geodesic dome arenas.
- **Unified Graph Interop:** Audio channels directly modulate visual particle forces, texture shaders, or DMX stage lights.
- **Zero Allocations:** Channel planar arrays stream at 60–240 Hz without memory reallocations.
