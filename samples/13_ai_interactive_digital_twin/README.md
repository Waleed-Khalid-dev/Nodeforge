# Neo Realms — Live AI Kinetic Digital Twin & Cyberpunk Style Arena

**Project:** `samples/13_ai_interactive_digital_twin/ai_interactive_digital_twin.nfp`  
**Target Hardware:** Live Camera (USB/UVC/NDI) / Nvidia/DirectML GPUs / Laser Projectors  
**Primary Engine Features:** `VideoDeviceInTexOp`, `NeuralStyleTexOp`, `SegmentationMaskTexOp`, `PoseEstimationChanOp`, `ParticleEmitterGeomOp`, `LaserGeomOp`, `LaserDACChanOp`, `SliderPanelComp`.

---

## 1. Overview

This project demonstrates **Real-Time Deep Learning Computer Vision, Fast Neural Style Transfer, 17-Keypoint Pose Tracking, and Kinetic Multi-Subsystem Synthesis** in NodeForge.

```
┌───────────────────────┐
│  VideoDeviceInTexOp   │ (Camera / Video Feed)
└───────────┬───────────┘
            ├──► NeuralStyleTexOp (Cyberpunk Style Transfer) ──► Final Video Out
            ├──► SegmentationMaskTexOp (Subject Alpha Matte)  ──►
            └──► PoseEstimationChanOp (17 Keypoints)
                     ├──► ParticleEmitterGeomOp (Hand-Tracked Particle Aura)
                     └──► LaserGeomOp ──► LaserDACChanOp (Laser Projected Skeleton)
```

---

## 2. Interactive Features

- **Fast Neural Style Transfer:** Live Cyberpunk Neon and Ink Wash stylization with sub-millisecond cook times.
- **17-Keypoint Body Tracking:** Optical skeletal tracking streaming joint coordinates for hands, head, and feet.
- **Green-Screen-Free Alpha Matting:** Automatic real-time subject background isolation.
- **Hand-Tracked Particle Swarms:** GPU particles attract to dancer hands.
- **Vectorized Laser Projection:** Real-time laser beams trace the performer skeleton.
