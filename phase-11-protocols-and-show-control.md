# Phase 11 Implementation Plan: Protocols & Show Control

## Executive Overview
Phase 11 establishes NodeForge's comprehensive hardware protocol and show-control foundation for live interactive installations, concerts, projection mapping shows, and architectural lighting. This phase adds low-latency, thread-safe input/output across **MIDI** (synthesizers, DJ controllers), **OSC** (TouchOSC, remote tablets, custom software), **Serial** (Arduino, ESP32, microcontrollers, custom sensors), **DMX512 / Art-Net 4** (stage lighting, dimmers, moving fixtures, LED bars), and **Interactive Input** (Keyboard and Mouse channels for stage hotkeys and kiosk interaction).

---

## Architecture Decisions & Requirements (Aligned with /grill-me)

- **Operator Scope:** Full Phase 11 suite: MIDI In/Out, OSC Real-Time ChanOps, Serial Comms, DMX/Art-Net 4 lighting, Keyboard/Mouse ChanOps.
- **MIDI Engine:** Asynchronous callback with lock-free ring buffer decoding CC (0-127), Notes (0-127), Pitch Bend, and Aftertouch into normalized [0..1] and raw [0..127] named channels (`ch1_cc1`, `ch1_note60`) plus trigger pulses.
- **OSC Real-Time Channels:** Dynamic address pattern mapping (e.g. `/speed` -> channel `speed`, `/pos` with 3 floats -> `pos_1`, `pos_2`, `pos_3`) with wildcard prefix filtering and decay/hold modes.
- **Serial Communication:** Asynchronous C++ worker thread with Win32 Overlapped I/O, supporting line-delimited ASCII strings (newline-delimited sensor lines into `DataTable` & channels), raw binary mode, configurable baud rate (9600–921600), and auto-reconnect.
- **DMX / Art-Net:** Native Art-Net 4 over UDP (Port 6454) broadcast/unicast protocol engine transmitting and receiving 512-channel DMX universes with auto-packet sequencing and SubNet/Universe routing (0–32767).
- **Keyboard & Mouse:** Multi-mode capture supporting Active Focused Window, Specific Projector Output Window, and Global Desktop capture via Win32 raw input APIs.

---

## Architectural Breakdown & Subphases

### Subphase 11.1 — Protocol I/O Subsystem Architecture & ADR-0011
- `docs/adr/ADR-0011-protocols-and-show-control.md`: Architectural decision record detailing threading model, lock-free ring buffers, real-time channel mapping, network sockets, and OS input hooks.
- `src/io/midi/MidiManager.h` & `MidiManager.cpp`: Hardware MIDI device enumeration, high-precision timestamped event ring buffer, message decoder.
- `src/io/serial/SerialPort.h` & `SerialPort.cpp`: Asynchronous Win32 Overlapped serial port communication, line parsing, auto-reconnect.
- `src/io/dmx/ArtNetEngine.h` & `ArtNetEngine.cpp`: Art-Net 4 standard packet encoder/decoder (`ArtDmx`), UDP port 6454 manager, 512-channel serializer.
- `src/io/input/InputManager.h` & `InputManager.cpp`: Aggregated input state for mouse and keyboard (local GLFW hooks + Win32 global desktop hooks).

### Subphase 11.2 — Operator Specifications (`docs/operator-spec/`)
Create formal specifications before implementation in accordance with roadmap rules:
- `docs/operator-spec/MIDIInChanOp.md`
- `docs/operator-spec/MIDIOutChanOp.md`
- `docs/operator-spec/OSCInChanOp.md`
- `docs/operator-spec/OSCOutChanOp.md`
- `docs/operator-spec/SerialDataOp.md`
- `docs/operator-spec/DMXInChanOp.md`
- `docs/operator-spec/DMXOutChanOp.md`
- `docs/operator-spec/MouseInChanOp.md`
- `docs/operator-spec/KeyboardInChanOp.md`

### Subphase 11.3 — Operator Implementations
- `src/operators/chan/MIDIInChanOp.h` & `.cpp`: Ingests MIDI events and outputs named channels with normalized range and trigger pulses.
- `src/operators/chan/MIDIOutChanOp.h` & `.cpp`: Sends MIDI CC / Notes on channel triggers.
- `src/operators/chan/OSCInChanOp.h` & `.cpp`: Non-blocking UDP receiver parsing OSC address trees into real-time channels.
- `src/operators/chan/OSCOutChanOp.h` & `.cpp`: Transmits channels as formatted OSC packets.
- `src/operators/data/SerialDataOp.h` & `.cpp`: Bidirectional COM port operator for `DataTable` and channels.
- `src/operators/chan/DMXInChanOp.h` & `.cpp`: Receives ArtDmx UDP packets and exposes 512 channels (`ch1`..`ch512`).
- `src/operators/chan/DMXOutChanOp.h` & `.cpp`: Encodes input channels into ArtDmx packets and broadcasts over LAN.
- `src/operators/chan/MouseInChanOp.h` & `.cpp`: Channels `x`, `y`, `dx`, `dy`, `left`, `right`, `middle`, `wheel_x`, `wheel_y`, `hover`.
- `src/operators/chan/KeyboardInChanOp.h` & `.cpp`: Channels for key states, modifier keys (`ctrl`, `shift`, `alt`), and pulse triggers.

### Subphase 11.4 — Engine Integration & Registration
- Register all 9 new operators in `src/graph/CoreNodes.cpp`.
- Expose protocol managers in `src/python/PyNodeForge.cpp`.
- Update `CMakeLists.txt` with required platform libraries (`ws2_32.lib`, `winmm.lib`).

### Subphase 11.5 — Automated Testing & Show Control Benchmarks
- `tests/unit/midi_protocol_test.cpp`: MIDI message framing, channel mapping, CC normalization, velocity tracking.
- `tests/unit/osc_channel_test.cpp`: Real-time OSC address tree parsing, wildcard pattern matching, multi-arg channel unpacking.
- `tests/unit/serial_protocol_test.cpp`: COM port configuration, ASCII line parsing, circular byte buffer.
- `tests/unit/dmx_artnet_test.cpp`: Art-Net 4 header parsing, universe routing, 512-channel byte serialization.
- `tests/unit/interactive_input_test.cpp`: Mouse coordinate normalization, delta tracking, keyboard state polling.
- `tests/benchmark/show_control_benchmark.cpp`: High-frequency stress test (10,000+ packets/sec across MIDI, OSC, Serial, Art-Net at 60 FPS) with 0 memory leaks.

### Subphase 11.6 — Verification & Status Sign-Off
- Run test runner to ensure 100% test pass rate.
- Update `STATUS.md` marking Phase 11 as complete.

---

## Verification Criteria
- [ ] All unit and benchmark tests pass cleanly with 100% pass rate.
- [ ] 0 memory leaks across continuous 10,000-frame cook loop with all protocols active.
- [ ] Live MIDI CC and Note triggers drive graph parameters.
- [ ] Real-time OSC and Art-Net packet transmission/reception verified.
- [ ] Clean compilation under MSVC `vcvars64.bat`.
