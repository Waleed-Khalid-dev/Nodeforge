# Implementation Plan — Phase 15 Epic 15.9: AI Neural Style Transfer & Real-Time ONNX/TensorRT Inference Engine

**Phase:** 15 (Expansion Toward “Almost Everything”)  
**Epic:** 15.9 (AI Neural Style Transfer & Real-Time TensorRT/ONNX Runtime Inference TexOp)  
**Status:** In Planning  
**Target Milestone:** M7 (Advanced Visual FX & Dynamic Tools)  
**Company:** Neo Realms  
**Owner:** Waleed Khalid  

---

## 1. Overview & Objectives

Epic 15.9 introduces an enterprise-grade **AI Neural Style Transfer & Real-Time Deep Learning Computer Vision Subsystem** to NodeForge. Built for live stage shows, interactive digital twins, real-time performer body tracking, green-screen-free subject segmentation, and artistic style transfer.

Key capabilities:
- **`ONNXInferenceEngine` (`src/ai/ONNXInferenceEngine.h/.cpp`)**: High-performance C++ AI inference engine wrapping ONNX runtime standards with DirectML / CUDA GPU execution providers, zero-copy NCHW tensor conversion, and an analytical neural transform fallback for headless CI testing.
- **`NeuralStyleTexOp` (Family: `TexOp`)**: Real-time Fast Neural Style Transfer operator with artistic presets (Candy, Mosaic, Rain Princess, Udnie, Cyberpunk Neon, Ink Wash) and continuous style blend weighting.
- **`ONNXInferenceTexOp` (Family: `TexOp`)**: General-purpose ONNX computer vision inference operator running arbitrary models (Super-Resolution, Depth Estimation, Background Matting) with input/output tensor binding.
- **`PoseEstimationChanOp` (Family: `ChanOp`)**: Deep learning human pose estimation node streaming 17 skeletal body keypoint coordinates (`nose, eyes, ears, shoulders, elbows, wrists, hips, knees, ankles`) into 2D/3D channels.
- **`SegmentationMaskTexOp` (Family: `TexOp`)**: Real-time background segmentation and green-screen-free subject masking for live camera feeds.
- **Cross-Subsystem Synergy**: Live camera feeds drive neural style transfer and pose estimation; hand keypoints attract GPU particles (`ParticleAttractorGeomOp`), body skeleton contours project as live vector beams via `LaserGeomOp`, and UI panels control style blending in real time (`PanelComp`).

---

## 2. Architecture & Neural Tensor Processing Model (ADR-0022)

### 2.1 NCHW Tensor Normalization
For input RGB image of dimensions $W \times H$ with pixel values $P_{x, y} \in [0, 255]$:

$$\begin{aligned}
T_{\text{norm}}[c, y, x] &= \frac{P[c, y, x] / 255.0 - \mu_c}{\sigma_c} \\
\mu &= [0.485, 0.456, 0.406], \quad \sigma = [0.229, 0.224, 0.225]
\end{aligned}$$

### 2.2 Style Transfer Blending
For original frame $I_{\text{src}}$, styled frame $I_{\text{style}}$, and style weight $\alpha \in [0.0, 1.0]$:

$$I_{\text{out}} = (1 - \alpha) \cdot I_{\text{src}} + \alpha \cdot I_{\text{style}}$$

---

## 3. Operator Suite & Specifications

### 1. `NeuralStyleTexOp` (Family: `TexOp`)
- **Input Pins:** `input` (Tex).
- **Output Pins:** `output` (Tex).
- **Parameters:**
  - `style_preset`: `0: Candy`, `1: Mosaic`, `2: Rain Princess`, `3: Udnie`, `4: Cyberpunk Neon`, `5: Ink Wash`.
  - `style_weight`: `float` (default: `1.0`, range: `0.0 .. 1.0`).
  - `preserve_colors`: `bool` (default: `false`).

### 2. `ONNXInferenceTexOp` (Family: `TexOp`)
- **Input Pins:** `input` (Tex).
- **Output Pins:** `output` (Tex).
- **Parameters:**
  - `model_path`: `string` (default: `""`).
  - `execution_provider`: `0: DirectML (GPU)`, `1: CUDA`, `2: CPU`.
  - `input_tensor_name`: `string` (default: `"input"`).
  - `output_tensor_name`: `string` (default: `"output"`).

### 3. `PoseEstimationChanOp` (Family: `ChanOp`)
- **Input Pins:** `input` (Tex, camera or video feed).
- **Output Pins:** `out_keypoints` (Chan, 17 keypoint XY channels), `out_confidence` (Chan, 17 confidence values).
- **Parameters:**
  - `confidence_threshold`: `float` (default: `0.5`, range: `0.0 .. 1.0`).
  - `smoothing`: `float` (default: `0.2`, range: `0.0 .. 0.9`).

### 4. `SegmentationMaskTexOp` (Family: `TexOp`)
- **Input Pins:** `input` (Tex).
- **Output Pins:** `output` (Tex, RGBA masked subject), `out_mask` (Tex, 1-channel grayscale matte).
- **Parameters:**
  - `threshold`: `float` (default: `0.5`, range: `0.0 .. 1.0`).
  - `feather`: `float` (default: `2.0`, range: `0.0 .. 10.0`).

---

## 4. Planned File Structure

```
d:\[Project]\Touch Designer\
├── docs\
│   ├── adr\
│   │   └── ADR-0022-ai-neural-style-and-inference.md
│   ├── operator-spec\
│   │   ├── NeuralStyleTexOp.md
│   │   ├── ONNXInferenceTexOp.md
│   │   ├── PoseEstimationChanOp.md
│   │   └── SegmentationMaskTexOp.md
│   └── cheat-sheets\
│       └── 15_ai_neural_cheat_sheet.md
├── src\
│   ├── ai\
│   │   ├── ONNXInferenceEngine.h/.cpp   # NCHW tensor conversion, ONNX session, neural transforms
│   │   └── PoseKeypoints.h             # 17-keypoint body joints definition
│   └── operators\
│       ├── tex\
│       │   ├── NeuralStyleTexOp.h/.cpp
│       │   ├── ONNXInferenceTexOp.h/.cpp
│       │   └── SegmentationMaskTexOp.h/.cpp
│       └── chan\
│           └── PoseEstimationChanOp.h/.cpp
├── samples\
│   └── 13_ai_interactive_digital_twin\
│       ├── ai_interactive_digital_twin.nfp # Flagship AI digital twin project
│       └── README.md
└── tests\
    ├── unit\
    │   └── ai_neural_test.cpp          # Unit tests for tensor math, style transfer, pose tracking
    └── benchmark\
        └── ai_neural_benchmark.cpp     # 60 FPS neural inference throughput benchmark
```

---

## 5. Detailed Task Breakdown

### Task 15.9.1: Architecture Documentation & Operator Specifications
- **Agent:** `project-planner` / `architecture`
- **Priority:** P0 (Design Pre-requisite)
- **Output:**
  1. `docs/adr/ADR-0022-ai-neural-style-and-inference.md`
  2. `docs/operator-spec/NeuralStyleTexOp.md`
  3. `docs/operator-spec/ONNXInferenceTexOp.md`
  4. `docs/operator-spec/PoseEstimationChanOp.md`
  5. `docs/operator-spec/SegmentationMaskTexOp.md`
  6. `docs/cheat-sheets/15_ai_neural_cheat_sheet.md`

### Task 15.9.2: AI Engine Core & Operator Implementations
- **Agent:** `backend-specialist` / `game-developer`
- **Priority:** P0 (Core Implementation)
- **Output:**
  1. `src/ai/PoseKeypoints.h`
  2. `src/ai/ONNXInferenceEngine.h/.cpp`
  3. `src/operators/tex/NeuralStyleTexOp.h/.cpp`
  4. `src/operators/tex/ONNXInferenceTexOp.h/.cpp`
  5. `src/operators/tex/SegmentationMaskTexOp.h/.cpp`
  6. `src/operators/chan/PoseEstimationChanOp.h/.cpp`
  7. Register all operators in `src/graph/CoreNodes.cpp` and `CMakeLists.txt`.

### Task 15.9.3: Showcase Sample Project (`samples/13_ai_interactive_digital_twin/`)
- **Agent:** `project-planner` / `backend-specialist`
- **Priority:** P1 (Production Sample)
- **Output:**
  1. `samples/13_ai_interactive_digital_twin/ai_interactive_digital_twin.nfp`
  2. `samples/13_ai_interactive_digital_twin/README.md`

### Task 15.9.4: Automated Unit Tests & 60 FPS Benchmark
- **Agent:** `test-engineer` / `backend-specialist`
- **Priority:** P0 (CI Quality Gate)
- **Output:**
  1. `tests/unit/ai_neural_test.cpp`
  2. `tests/benchmark/ai_neural_benchmark.cpp`
  3. Update `tests/CMakeLists.txt`.

---

## 6. Phase X: Final Verification Checklist

- [ ] `docs/adr/ADR-0022-ai-neural-style-and-inference.md` written and approved
- [ ] Operator specs for all 4 AI operators created in `docs/operator-spec/`
- [ ] `ONNXInferenceEngine`, `NeuralStyleTexOp`, `ONNXInferenceTexOp`, `PoseEstimationChanOp`, `SegmentationMaskTexOp` implemented
- [ ] Operators registered in `CoreNodes.cpp` and `CMakeLists.txt`
- [ ] `samples/13_ai_interactive_digital_twin/ai_interactive_digital_twin.nfp` created with documentation
- [ ] `tests/unit/ai_neural_test.cpp` and `tests/benchmark/ai_neural_benchmark.cpp` added to CMake
- [ ] 60 FPS AI neural inference benchmark verified with zero memory leaks
- [ ] All automated tests passing (100% pass rate)
- [ ] `STATUS.md` and `.agent/memory/MEMORY.md` updated
