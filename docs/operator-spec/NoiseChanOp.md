# Operator Specification: NoiseChanOp

**Family:** `ChanOp` (Channel Operator)  
**Name:** `Noise`  
**Description:** Generates continuous pseudo-random smooth noise curves (1D Perlin / Simplex / Fractal Brownian Motion).

---

## Inputs & Outputs

### Input Pins
- None (Generator Node)

### Output Pins
- `output` (`PinType::Chan` / `nf::ChannelBuffer`): Multi-channel procedural noise stream.

---

## Parameters

| Name | Type | Default | Description |
|---|---|---|---|
| `seed` | `int32_t` | 0 | Random seed |
| `roughness` | `float` | 0.5 | Fractal octaves roughness |
| `period` | `float` | 1.0 | Noise spatial/temporal wavelength in seconds |
| `amplitude` | `float` | 1.0 | Output scaling |
| `offset` | `float` | 0.0 | Output bias |
| `octaves` | `int32_t` | 3 | Number of fractal noise octaves |
| `channel_names` | `std::string` | `"noise1"` | Space-separated list of channel names |
| `time_sliced` | `bool` | `true` | Live continuous streaming vs static buffer |
| `sample_count` | `int32_t` | 1 | Number of samples if static |
| `sample_rate` | `float` | 60.0 | Sample rate in Hz |

---

## Cook Behavior
1. Samples 1D gradient noise function using temporal coordinate $x = t / \text{period} + \text{channel\_seed}$.
2. Evaluates fractal Brownian motion (FBM) sum: $\sum_{k=0}^{\text{octaves}-1} r^k \cdot \text{noise}(2^k x)$.
3. Scales and biases signal to output channel buffer.
