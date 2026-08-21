# ADR-0021: Laser DAC & ILDA Laser Projector Control

**Status:** Accepted  
**Date:** 2026-08-21  
**Author:** Waleed Khalid  
**Deciders:** Core Architecture Team  
**Phase:** 15 (Expansion Toward “Almost Everything” — Epic 15.8)  

---

## 1. Context & Problem Statement

High-impact live concert visuals, architectural laser mapping, kinetic beam sculptures, and vector graphics require real-time vector point streaming (12 kpps to 100 kpps) to galvanometer laser projectors via hardware Digital-to-Analog Converters (DACs) such as Ether Dream (Ethernet) and Helios (USB).

Raster video cannot drive galvanometer scanners. Scanners operate on mechanical mirror galvos with finite inertia, requiring blanking delays between separate vector paths, corner dwell times, and velocity clamping to prevent mechanical damage and image distortion.

---

## 2. Decision & Architecture

### 2.1 ILDA Point Standard & Coordinate Mapping
ILDA coordinates are 16-bit signed integers in $[-32768, 32767]$ with normalized RGB color and status flags:

$$\begin{aligned}
X_{16} &= \text{clamp}\left(\text{round}\left(x_{\text{norm}} \times 32767\right), -32768, 32767\right) \\
Y_{16} &= \text{clamp}\left(\text{round}\left(y_{\text{norm}} \times 32767\right), -32768, 32767\right) \\
\text{Flags} &= \text{isBlanked} \,?\, 0x40 \,:\, 0x00
\end{aligned}$$

### 2.2 Galvo Path Optimization
- **Blanking Transition:** When jumping between disconnected contours, insert $N_{\text{pre}}$ blanked points at departure, $N_{\text{travel}}$ transit points, and $N_{\text{post}}$ blanked points at arrival.
- **Corner Dwell:** At sharp turns $\theta < \theta_{\text{thresh}}$, insert $N_{\text{dwell}}$ points so galvo mirrors settle accurately.

### 2.3 Modular Operator Suite
- `LaserGeomOp` (Family: `GeomOp`): Converts 3D geometry and vector contours into optimized ILDA laser point paths.
- `LaserDACChanOp` (Family: `ChanOp`): Streams point buffers to Ether Dream / Helios DACs with safety interlock shutter.
- `LaserPatternGeomOp` (Family: `GeomOp`): Generates classic laser lissajous spirals, spirographs, and multi-beam fans.
- `LaserFileInGeomOp` (Family: `GeomOp`): Loads standard `.ild` vector animation files.

---

## 3. Consequences

### Positive
- **Hardware Agnostic:** Supports Ether Dream (network) and Helios (USB) DACs with headless emulation fallback.
- **Galvo Safety:** Prevents mirror overshoot and scanner coil overheating via automatic blanking and corner dwells.
- **Multi-Subsystem Synergy:** Seamlessly integrates with volumetric smoke atmospheres and audio-reactive waveforms.
