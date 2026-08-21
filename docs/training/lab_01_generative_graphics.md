# Workshop Lab 1: Generative Motion Graphics & Shader Pipelines

**Module:** 2 | **Objective:** Build an audio-reactive generative liquid ripple shader from scratch.

---

## 1. Step-by-Step Build Instructions

1. **Place Master Clock:**
   - Press `Tab`, type `TimeChanOp`, and place it on the canvas.
   - Connect its `time` output pin to the input of a new `LFOChanOp`.
2. **Configure LFO:**
   - Select `LFOChanOp`. In the Parameter Inspector, set:
     - `type`: `Sine` (0)
     - `frequency`: `0.25`
     - `amplitude`: `1.0`
3. **Create Generative Noise Texture:**
   - Press `Tab`, create a `NoiseTexOp`.
   - Set `type` to `Perlin` (1), `frequency` to `3.5`, and `harmonics` to `3`.
   - Toggle the `time` parameter to Expression mode (`E`) and enter:
     ```python
     op('lfo1')['chan1'] * 4.0
     ```
4. **Create Separable Glow Filter:**
   - Create a `BlurTexOp` and wire `noise1.texture` -> `blur1.input`.
   - Set `radius` = `16`, `sigma` = `6.0`, `passes` = `2`.
5. **Blend Elements:**
   - Create a `CompositeTexOp`.
   - Wire `noise1.texture` -> `composite1.input_a` and `blur1.output` -> `composite1.input_b`.
   - Set `operation` = `Add` (1) and `opacity` = `0.85`.
6. **Connect Viewport Output:**
   - Create a `ToWindowTexOp` and wire `composite1.output` -> `towindow1.input`.
   - You should see a live 60 FPS glowing liquid motion graphic!

---

## 2. Self-Assessment Challenge

- Add a `LevelTexOp` after `composite1` to boost gamma and contrast.
- Bind the mouse cursor X position (`MouseInChanOp`) to control `blur1.radius` in real-time.
