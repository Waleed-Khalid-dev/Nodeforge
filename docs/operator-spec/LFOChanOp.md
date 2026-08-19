# Operator Specification: LFOChanOp

**Family:** `ChanOp` (Channel Operator)  
**Name:** `LFO`  
**Description:** Generates periodic low-frequency waveforms (Sine, Triangle, Sawtooth, Square, Pulse, Perlin Wave) for procedural parameter animation and modulation.

---

## Inputs & Outputs

### Input Pins
- `reset` (`PinType::Chan`, Optional): Reset phase trigger signal.

### Output Pins
- `output` (`PinType::Chan` / `nf::ChannelBuffer`): Multi-channel waveform buffer.

---

## Parameters

| Name | Type | Default | Description |
|---|---|---|---|
| `type` | `int32_t` (Menu) | 0 | Waveform type: `0: Sine`, `1: Triangle`, `2: Saw`, `3: Square`, `4: Pulse`, `5: Perlin` |
| `frequency` | `float` | 1.0 | Frequency in Hz (cycles per second) |
| `amplitude` | `float` | 1.0 | Waveform peak amplitude |
| `offset` | `float` | 0.0 | DC offset added to output signal |
| `phase` | `float` | 0.0 | Phase shift in normalized cycles (0.0 to 1.0) |
| `pulse_width` | `float` | 0.5 | Duty cycle for Square / Pulse waveforms |
| `channel_names` | `std::string` | `"chan1"` | Space-separated list of output channel names |
| `time_sliced` | `bool` | `true` | Generate live time slice vs full-range preview curve |
| `sample_count` | `int32_t` | 1 | Number of samples per channel (when not time-sliced) |
| `sample_rate` | `float` | 60.0 | Sample rate in Hz |

---

## Cook Behavior
1. Computes instantaneous phase from timeline seconds or internal phase accumulator $\phi = 2\pi (f \cdot t + \text{phase})$.
2. Evaluates the selected mathematical waveform equation across the sample window.
3. Applies amplitude scale and DC offset: $y(t) = \text{offset} + \text{amplitude} \cdot W(\phi)$.
4. Writes multi-channel payload to `output` pin.
