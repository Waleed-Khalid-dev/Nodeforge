# Neo Realms — Generative Audio-Visual Stage Synthesizer

**Project:** `samples/03_audiovisual_stage/audiovisual_stage.nfp`  
**Target Hardware:** Live Concert Stage Displays / LED Video Wall / VJ Setup  
**Primary Engine Features:** `AudioFileInChanOp`, `MathChanOp`, `FilterChanOp`, `MIDIInChanOp`, `ChanToTexOp`, `NoiseTexOp`, `CompositeTexOp`, `LevelTexOp`, `ToWindowTexOp`.

---

## 1. Overview

This project implements an audio-reactive visual synthesizer engineered for live performance and concert visuals. Audio spectrum data is captured in real-time by `AudioFileInChanOp`, analyzed with SIMD RMS smoothing filters, rasterized into a dynamic GPU texture via `ChanToTexOp`, and blended with multi-harmonic procedural noise textures. Hardware MIDI CC faders allow live performers to modulate noise frequencies, color palettes, and glow intensities.

```
       [ Audio File / Live Mic ]            [ USB MIDI Controller ]
                  │                                    │
                  ▼                                    ▼
       ┌─────────────────────┐              ┌─────────────────────┐
       │  AudioFileInChanOp  │              │    MIDIInChanOp     │
       └──────────┬──────────┘              └──────────┬──────────┘
                  │                                    │
       ┌──────────▼──────────┐                         │
       │     MathChanOp      │ (Gain Scale)            │
       └──────────┬──────────┘                         │
                  │                                    │
       ┌──────────▼──────────┐                         │
       │    FilterChanOp     │ (Peak Envelope)         │
       └──────────┬──────────┘                         │
                  │                                    │
                  ├────────────────────────┐           │ (Expression Modulation)
                  ▼                        ▼           ▼
       ┌─────────────────────┐      ┌─────────────────────┐
       │    ChanToTexOp      │      │     NoiseTexOp      │
       │  (Spectrum Texture) │      │ (Kaleidoscope Mode) │
       └──────────┬──────────┘      └──────────┬──────────┘
                  │                            │
                  └─────────────┬──────────────┘
                                ▼
                     ┌─────────────────────┐
                     │   CompositeTexOp    │ (Screen / Add Mode)
                     └──────────┬──────────┘
                                ▼
                     ┌─────────────────────┐
                     │     LevelTexOp      │ (Color Grade & Contrast)
                     └──────────┬──────────┘
                                ▼
                     ┌─────────────────────┐
                     │    ToWindowTexOp    │ (60 FPS Stage Output)
                     └─────────────────────┘
```

---

## 2. MIDI Controller Mapping

| MIDI CC # | Channel | Parameter Target | Function |
|-----------|---------|------------------|----------|
| CC 1 (Modulation) | 1 | `kaleido_noise.frequency` | Waveform density & complexity |
| CC 7 (Volume) | 1 | `audio_stream.volume` | Audio input gain / reaction threshold |
| CC 10 (Pan) | 1 | `stage_level.contrast` | Visual punch & color grading |
| CC 16 (Knob 1) | 1 | `stage_composite.opacity`| Blend ratio between spectrum & noise |
