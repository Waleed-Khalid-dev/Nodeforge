# Neo Realms — Interactive VR Spatial Holodeck & Kinetic Gesture Arena

**Project:** `samples/10_openxr_immersive_holodeck/openxr_immersive_holodeck.nfp`  
**Target Hardware:** Meta Quest (Link/AirLink) / HTC Vive / Valve Index / Apple Vision Pro OpenXR Streaming  
**Primary Engine Features:** `OpenXRHeadsetTexOp`, `OpenXRControllerChanOp`, `OpenXRHandTrackingChanOp`, `OpenXRCameraComp`, `AudioListenerComp`, `AudioSpatializerChanOp`, `GridGeomOp`, `RenderTexOp`.

---

## 1. Overview

This project demonstrates real-time **OpenXR 1.0 Spatial Computing & Dual-Eye Stereoscopic Presentation** inside the NodeForge DAG. 

The user experiences a 3D virtual holodeck with 6-DOF head tracking, dual motion controllers, articulated 26-joint skeletal hand gesture recognition, and binaural 3D spatialized audio.

```
┌────────────────────────┐
│    OpenXRCameraComp    │ (Tracks HMD Head Pose: tx, ty, tz, rx, ry, rz, rw)
└───────────┬────────────┘
            │
            ├──────────────────────────────────────────┐
            ▼                                          ▼
┌────────────────────────┐                 ┌────────────────────────┐
│  RenderTexOp (Left)    │                 │  RenderTexOp (Right)   │
└───────────┬────────────┘                 └───────────┬────────────┘
            │                                          │
            └────────────────────┬─────────────────────┘
                                 │
                                 ▼
                    ┌────────────────────────┐
                    │  OpenXRHeadsetTexOp    │ (Presents Stereo to VR HMD)
                    └────────────────────────┘
```

---

## 2. Interactive Features

- **Stereoscopic Dual-Eye Rendering:** 90–120 FPS left/right eye presentation with IPD calculation.
- **6-DOF Motion Controllers:** Real-time analog triggers, thumbsticks, and hand poses.
- **26-Joint Skeletal Hand Tracking:** Articulated hand joints and pinch gesture recognition.
- **3D Spatial Binaural Audio:** Head pose automatically drives the virtual listener.
