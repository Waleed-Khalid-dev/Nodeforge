# Operator Specification: AudioFileInChanOp

**Family:** `ChanOp` (Channel Operator)  
**Name:** `AudioFileIn`  
**Description:** Decodes audio files (WAV, MP3, FLAC, AIFF) into multi-channel audio buffers with continuous streaming or full-range waveform playback.

---

## Inputs & Outputs

### Input Pins
- None (Generator Node)

### Output Pins
- `output` (`PinType::Chan` / `nf::ChannelBuffer`): Multi-channel audio sample stream (typically `chan1`, `chan2` for L/R).

---

## Parameters

| Name | Type | Default | Description |
|---|---|---|---|
| `file_path` | `std::string` | `""` | Path to audio file |
| `play` | `bool` | `true` | Enable playback |
| `loop` | `bool` | `true` | Loop audio when reaching the end |
| `volume` | `float` | 1.0 | Gain multiplier |
| `time_sliced` | `bool` | `true` | Stream live audio chunks per frame vs full file buffer |
| `sample_rate` | `float` | 44100.0 | Output audio sample rate in Hz |

---

## Cook Behavior
1. Opens and decodes audio file header and PCM samples via audio decoding subsystem.
2. If `time_sliced`: streams $N = \text{sample\_rate} \times \Delta t$ samples for current frame time.
3. If full range: outputs entire audio track buffer.
4. Normalizes amplitude per `volume` and populates L/R channels on `output`.
