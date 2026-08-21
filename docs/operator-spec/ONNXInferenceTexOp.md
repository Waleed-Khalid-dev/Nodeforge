# Operator Specification: ONNXInferenceTexOp

**Family:** `TexOp`  
**Type Name:** `ONNXInferenceTexOp`  
**Version:** 1.0  
**Phase Ownership:** Phase 15 (Epic 15.9)  

---

## 1. Description

`ONNXInferenceTexOp` executes arbitrary ONNX deep learning models (Super-Resolution, Monocular Depth, Colorization) directly on GPU texture inputs.

---

## 2. Pins

### Input Pins
| Pin Name | Type | Required | Description |
|---|---|---|---|
| `input` | `Tex` | Yes | Input texture tensor |

### Output Pins
| Pin Name | Type | Description |
|---|---|---|
| `output` | `Tex` | Output tensor rendered to texture |

---

## 3. Parameters

| Parameter | Type | Default | Range | Description |
|---|---|---|---|---|
| `model_path` | `string` | `""` | - | Path to `.onnx` deep learning model |
| `execution_provider` | `int` | `0` | `0 .. 2` | 0: DirectML (GPU), 1: CUDA, 2: CPU |
| `input_tensor_name` | `string` | `"input"` | - | Input tensor name |
| `output_tensor_name` | `string` | `"output"` | - | Output tensor name |

---

## 4. Cook Semantics

- **Time-Dependent:** Yes.
