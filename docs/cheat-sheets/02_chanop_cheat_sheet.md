# ChanOp Family Cheat Sheet (1D/SIMD Channels & Show Control)

**Family:** `ChanOp` | **Color:** Green (`#00E676`) | **Data Structure:** `ChannelBuffer` (Contiguous SIMD float array)

---

## 1. ChanOp Architecture

ChanOps manipulate named streams of numeric channel data (e.g. `chan1`, `chan2`, `x`, `y`, `z`, `r`, `g`, `b`). All operations use vectorized AVX2/SSE SIMD mathematics for microsecond evaluation speeds.

---

## 2. Core Operator Reference (17 Operators)

| Operator | In Pins | Out Pins | Key Parameters | Cook Behavior |
|----------|---------|----------|----------------|---------------|
| **`ConstantChanOp`** | None | `output` (Chan) | `value0`, `value1`, `value2`, `channel_names` | Generates constant numeric channels |
| **`TimeChanOp`** | None | `time`, `frame`, `progress` | `speed` (float) | Outputs global timeline clock channels |
| **`LFOChanOp`** | `phase` (optional) | `output` (Chan) | `type` (Sine, Tri, Saw, Square, Pulse), `frequency`, `amplitude`, `bias` | Periodic waveform oscillator |
| **`NoiseChanOp`** | None | `output` (Chan) | `type` (Perlin, Simplex, Random), `period`, `amplitude`, `seed` | Smooth 1D procedural noise channel |
| **`MathChanOp`** | `input` (Chan) | `output` (Chan) | `operation` (Add, Sub, Mul, Div, Sin, Cos), `scalar`, `clamp_min`, `clamp_max` | Arithmetic channel transform |
| **`FilterChanOp`** | `input` (Chan) | `output` (Chan) | `type` (Lag, Spring, Damping, Box), `filter_width` | Smooths noise and sudden spikes |
| **`MergeChanOp`** | `input_a`, `input_b` | `output` (Chan) | None | Merges multiple channel sets into one |
| **`SelectChanOp`** | `input` (Chan) | `output` (Chan) | `pattern` (e.g. `chan*`, `x y z`), `rename` | Extracts and renames specific channels |
| **`TrailChanOp`** | `input` (Chan) | `output` (Chan) | `window_size` (int), `sample_rate` (float) | Sliding ring buffer for oscilloscopes |
| **`AudioFileInChanOp`**| None | `output` (Chan) | `file_path` (string), `play`, `volume`, `loop` | Reads audio streams into channel buffers |
| **`TexToChanOp`** | `input` (Tex) | `output` (Chan) | `sample_mode` (Row, Col, Average, Point) | Converts 2D pixel lines into 1D channels |
| **`ChanToTexOp`** | `input` (Chan) | `output` (Tex) | `fit_mode`, `height`, `format` | Converts channel buffer into a GPU texture |
| **`MIDIInChanOp`** | None | `output` (Chan) | `device_index` (int), `channel` (int) | Captures USB MIDI CC and Notes in real-time |
| **`MIDIOutChanOp`** | `input` (Chan) | None | `device_index` (int), `channel` (int) | Transmits MIDI CC / Note messages |
| **`OSCInChanOp`** | None | `output` (Chan) | `port` (int), `address_pattern` (string) | Decodes binary OSC UDP packets into channels |
| **`OSCOutChanOp`** | `input` (Chan) | None | `ip_address` (string), `port` (int), `address_pattern` | Broadcasts channels over OSC UDP packets |
| **`DMXInChanOp`** | None | `output` (Chan) | `port` (int), `universe` (int) | Receives 512-channel Art-Net 4 DMX packets |
| **`DMXOutChanOp`** | `input` (Chan) | None | `ip_address` (string), `port` (int), `universe` (int) | Broadcasts Art-Net 4 DMX512 universe frames |
| **`MouseInChanOp`** | None | `output` (Chan) | `scope` (Screen/Window), `normalize` (bool) | Tracks mouse cursor X/Y coordinates and buttons |
| **`KeyboardInChanOp`** | None | `output` (Chan) | `keys` (string space-separated) | Outputs binary `0.0 / 1.0` keypress channels |

---

## 3. Common Idioms

```
# Sensor Smoothing & Clamping
OSCInChanOp ──► FilterChanOp (Lag: 0.15s) ──► MathChanOp (Clamp: 0.0 .. 1.0) ──► [Param Binding]

# Audio Spectrum to Dynamic Visual Ribbon
AudioFileInChanOp ──► MathChanOp (Scale: 2.0) ──► ChanToTexOp ──► BlurTexOp ──► StageVisualizer
```
