# Operator Specification: PoseEstimationChanOp

**Family:** `ChanOp`  
**Type Name:** `PoseEstimationChanOp`  
**Version:** 1.0  
**Phase Ownership:** Phase 15 (Epic 15.9)  

---

## 1. Description

`PoseEstimationChanOp` runs deep learning human pose estimation on video inputs, extracting 17 standard body keypoints (`nose, eyes, ears, shoulders, elbows, wrists, hips, knees, ankles`) into 2D normalized channel tracks.

---

## 2. Pins

### Input Pins
| Pin Name | Type | Required | Description |
|---|---|---|---|
| `input` | `Tex` | Yes | Input video or live camera texture |

### Output Pins
| Pin Name | Type | Description |
|---|---|---|
| `out_keypoints` | `Chan` | 17-keypoint XY coordinates stream (34 channels) |
| `out_confidence` | `Chan` | 17 keypoint confidence scores |

---

## 3. Parameters

| Parameter | Type | Default | Range | Description |
|---|---|---|---|---|
| `confidence_threshold` | `float` | `0.5` | `0.0 .. 1.0` | Minimum optical tracking confidence |
| `smoothing` | `float` | `0.2` | `0.0 .. 0.9` | Temporal exponential smoothing factor |

---

## 4. Cook Semantics

- **Time-Dependent:** Yes.
