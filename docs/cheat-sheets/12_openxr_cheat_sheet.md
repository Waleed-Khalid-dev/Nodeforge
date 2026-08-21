# OpenXR VR & AR Subsystem Cheat Sheet

**Subsystem:** OpenXR Spatial Tracking & Stereoscopic VR/AR | **Version:** 1.0 (Phase 15 Epic 15.6)

---

## 1. OpenXR Operator Reference

| Operator | Family | In Pins | Out Pins | Key Roles |
|---|---|---|---|---|
| **`OpenXRHeadsetTexOp`** | `TexOp` | `in_left_eye` (Tex), `in_right_eye` (Tex) | `output` (Tex) | Dual-eye stereoscopic HMD presenter & 2D companion preview |
| **`OpenXRControllerChanOp`** | `ChanOp` | None | `out_left` (Chan), `out_right` (Chan) | 6-DOF controller tracking, buttons, and trigger feeds |
| **`OpenXRHandTrackingChanOp`** | `ChanOp` | None | `out_skeleton` (Chan), `out_gestures` (Chan) | 26-joint skeletal hand articulation & pinch tracking |
| **`OpenXRCameraComp`** | `Comp` | None | `output` (Comp), `out_head_pose` (Chan) | Synchronized 3D stereoscopic HMD camera rig |

---

## 2. Standard Network Wiring Idiom

```
# Interactive VR Holodeck with 3D Spatial Audio & Gesture Particles
OpenXRCameraComp (HMD Head) ───────┬─► Scene3DPass (Left/Right Eye) ──► OpenXRHeadsetTexOp (VR Goggles)
                                    │
                                    └─► AudioListenerComp (3D Binaural Ears)

OpenXRControllerChanOp (Right Hand) ─► ParticleAttractorGeomOp (Pulls GPU Particles)
                                    │
                                    └─► PhysicsForceChanOp (Blasts Rigid Bodies)
```
