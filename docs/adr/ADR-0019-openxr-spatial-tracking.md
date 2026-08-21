# ADR-0019: OpenXR / VR & AR Spatial Tracking & Stereoscopic Presentation

**Status:** Accepted  
**Date:** 2026-08-21  
**Author:** Waleed Khalid  
**Deciders:** Core Architecture Team  
**Phase:** 15 (Expansion Toward “Almost Everything” — Epic 15.6)  

---

## 1. Context & Problem Statement

Immersive virtual reality (VR), augmented reality (AR), and mixed reality (MR) installations require real-time 6-DOF spatial tracking of head-mounted displays (HMDs), dual motion controllers, and articulated 26-joint skeletal hand gestures.

Proprietary vendor SDKs (such as SteamVR OpenVR or Oculus PC SDK) cause heavy lock-in and binary dependencies. NodeForge adheres to the **OpenXR 1.0** open standard from Khronos, enabling hardware-agnostic connectivity across Meta Quest (Link/AirLink), HTC Vive, Valve Index, and Apple Vision Pro streaming, while providing an automated headless emulation fallback for CI testing.

---

## 2. Decision & Architecture

### 2.1 Stereoscopic Asymmetric FOV Projection Matrix
OpenXR provides asymmetric FOV angles for each eye: $\text{angleLeft}, \text{angleRight}, \text{angleUp}, \text{angleDown}$.

Projection matrix $\mathbf{P}$:

$$\begin{aligned}
\tan L &= \tan(\text{angleLeft}), \quad \tan R = \tan(\text{angleRight}) \\
\tan U &= \tan(\text{angleUp}), \quad \tan D = \tan(\text{angleDown}) \\
\mathbf{P}_{0,0} &= \frac{2}{\tan R - \tan L}, \quad \mathbf{P}_{2,0} = \frac{\tan R + \tan L}{\tan R - \tan L} \\
\mathbf{P}_{1,1} &= \frac{2}{\tan U - \tan D}, \quad \mathbf{P}_{2,1} = \frac{\tan U + \tan D}{\tan U - \tan D} \\
\mathbf{P}_{2,2} &= -\frac{z_f + z_n}{z_f - z_n}, \quad \mathbf{P}_{3,2} = -\frac{2 \cdot z_f \cdot z_n}{z_f - z_n} \\
\mathbf{P}_{2,3} &= -1.0, \quad \mathbf{P}_{3,3} = 0.0
\end{aligned}$$

### 2.2 Dual-Eye View Matrix with IPD Offset
For head pose $(\vec{P}_{\text{head}}, \mathbf{Q}_{\text{head}})$ and eye offset vector $\vec{O}_e = (\pm \text{IPD}/2, 0, 0)$:

$$\vec{P}_e = \vec{P}_{\text{head}} + \mathbf{Q}_{\text{head}} \cdot \vec{O}_e, \quad \mathbf{V}_e = (\mathbf{R}(\mathbf{Q}_{\text{head}}))^{-1} \cdot \mathbf{T}(-\vec{P}_e)$$

### 2.3 Modular Operator Suite
- `OpenXRHeadsetTexOp` (Family: `TexOp`): Stereoscopic HMD presenter rendering left/right eyes into OpenXR swapchains with companion 2D preview.
- `OpenXRControllerChanOp` (Family: `ChanOp`): Streams 6-DOF tracking transforms (`tx, ty, tz, rx, ry, rz, rw`) and button inputs for Left and Right controllers.
- `OpenXRHandTrackingChanOp` (Family: `ChanOp`): Streams 26 skeletal hand joint transforms per hand.
- `OpenXRCameraComp` (Family: `Comp`): Stereoscopic camera rig synchronized with HMD head pose in 3D world space.

---

## 3. Consequences

### Positive
- **Standardized & Future-Proof:** Based on open Khronos OpenXR 1.0 specification.
- **Headless CI Capable:** Automated emulation mode allows 100% automated test verification without physical VR goggles.
- **Cross-Subsystem Synergy:** Directly drives 3D spatial audio listener, particle forces, and in-graph UI panels.
