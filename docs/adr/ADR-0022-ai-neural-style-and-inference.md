# ADR-0022: AI Neural Style Transfer & Real-Time ONNX Inference Engine

**Status:** Accepted  
**Date:** 2026-08-21  
**Author:** Waleed Khalid  
**Deciders:** Core Architecture Team  
**Phase:** 15 (Expansion Toward “Almost Everything” — Epic 15.9)  

---

## 1. Context & Problem Statement

Next-generation generative media installations, virtual production stage broadcasts, and interactive dance performances require real-time deep learning computer vision (60 FPS) directly inside the visual node graph.

Traditional Python-based PyTorch scripts incur heavy IPC overhead and frame dropping when transferring 4K/60fps textures between processes. NodeForge requires a native C++ deep learning inference runtime wrapping the **ONNX Runtime (DirectML / CUDA / CPU)** standard with zero-copy GPU texture interop and built-in analytical fallbacks for CI testing.

---

## 2. Decision & Architecture

### 2.1 Tensor Normalization & Dataflow
Textures are converted between GPU `Texture2D` and 4D float tensors $\mathbf{T} \in \mathbb{R}^{B \times C \times H \times W}$:

$$T_{\text{norm}}[c, y, x] = \frac{P[c, y, x] / 255.0 - \mu_c}{\sigma_c}$$

with ImageNet standard means $\mu = [0.485, 0.456, 0.406]$ and std deviations $\sigma = [0.229, 0.224, 0.225]$.

### 2.2 Fast Neural Style Transfer & Blending
Real-time feedforward convolutional neural networks transform input textures into distinct artistic styles (Candy, Mosaic, Rain Princess, Udnie, Cyberpunk Neon, Ink Wash) with continuous interpolation:

$$I_{\text{out}} = (1 - \alpha) \cdot I_{\text{src}} + \alpha \cdot I_{\text{style}}$$

### 2.3 Modular Operator Suite
- `NeuralStyleTexOp` (Family: `TexOp`): Real-time Fast Neural Style Transfer with artistic presets and blend weighting.
- `ONNXInferenceTexOp` (Family: `TexOp`): General-purpose ONNX computer vision model runner.
- `PoseEstimationChanOp` (Family: `ChanOp`): 17-keypoint human body pose tracker streaming channels (`x, y, confidence`).
- `SegmentationMaskTexOp` (Family: `TexOp`): Green-screen-free background segmentation and subject alpha matting.

---

## 3. Consequences

### Positive
- **Real-Time 60 FPS Performance:** C++ zero-copy GPU tensor conversion avoids Python process bottlenecks.
- **Hardware Acceleration:** Native DirectML / CUDA support for Nvidia, AMD, and Intel GPUs with CPU fallback.
- **Rich Multi-Subsystem Synergy:** Body keypoints drive GPU compute particles, laser vector beams, and UI panels.
