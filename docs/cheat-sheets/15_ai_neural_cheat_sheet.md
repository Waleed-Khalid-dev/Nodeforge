# AI Neural Vision & Inference Subsystem Cheat Sheet

**Subsystem:** AI Neural Style Transfer & ONNX Runtime Inference | **Version:** 1.0 (Phase 15 Epic 15.9)

---

## 1. AI Operator Reference

| Operator | Family | In Pins | Out Pins | Key Roles |
|---|---|---|---|---|
| **`NeuralStyleTexOp`** | `TexOp` | `input` (Tex) | `output` (Tex) | Fast artistic style transfer with presets & blend weighting |
| **`ONNXInferenceTexOp`** | `TexOp` | `input` (Tex) | `output` (Tex) | General-purpose arbitrary ONNX model runner |
| **`PoseEstimationChanOp`** | `ChanOp` | `input` (Tex) | `out_keypoints` (Chan), `out_confidence` (Chan) | 17-keypoint human body pose tracker stream |
| **`SegmentationMaskTexOp`** | `TexOp` | `input` (Tex) | `output` (Tex), `out_mask` (Tex) | Green-screen-free human background segmentation |

---

## 2. Standard Network Wiring Idiom

```
# Live Interactive AI Digital Twin with Particle Aura & Laser Skeleton
VideoDeviceInTexOp ───────────┬─► NeuralStyleTexOp (Cyberpunk Style) ──► CompositeTexOp ──► Output
                              │                                                ▲
                              ├─► SegmentationMaskTexOp (Subject Alpha) ───────┘
                              │
                              └─► PoseEstimationChanOp (17 Keypoints)
                                       │
                                       ├─► ParticleAttractorGeomOp (Hand Aura)
                                       └─► LaserGeomOp (Laser Projected Skeleton)
```
