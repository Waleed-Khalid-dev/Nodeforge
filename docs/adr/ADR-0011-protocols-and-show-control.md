# ADR-0011: Protocols & Show Control Architecture (MIDI, OSC, Serial, DMX/Art-Net, Interactive Input)

## Status
Accepted (Phase 11)

## Context
NodeForge is designed for high-performance live multimedia, projection mapping, interactive kiosks, and theatrical show control. Live performances require robust, sub-millisecond, jitter-free ingestion and transmission of control signals from diverse industry standards:
1. **MIDI**: Synthesizers, motorized fader controllers, foot pedals, electronic drums (16 channels, CC, Note On/Off, Pitch Bend, Aftertouch).
2. **OSC (Open Sound Control)**: Tablet surfaces (TouchOSC), spatial audio controllers, network endpoints over UDP.
3. **Serial (RS-232 / USB COM)**: Microcontrollers (Arduino, ESP32, Teensy), distance/ultrasonic sensors, floor pressure plates.
4. **DMX512 / Art-Net 4**: Stage lighting, moving fixtures, strobes, LED bars, lasers over standard UDP Port 6454.
5. **Interactive Input**: Mouse and Keyboard channel controllers for stage triggers, kiosk displays, and multi-display interaction.

## Architecture Decisions

### 1. Threading & Decoupled Ingestion Model
To prevent network or serial I/O latency from stalling the 60 FPS GPU/Graph cook loop:
- **Dedicated Worker Threads / Background Callbacks**: MIDI callbacks (WinMM/RtMidi), OSC UDP listener sockets, Serial Overlapped I/O workers, and Art-Net UDP sockets execute asynchronously in worker threads.
- **Lock-Free / Mutex-Guarded Ring Buffers**: Incoming messages are placed into high-throughput ring buffers and state lookup tables.
- **Deterministic Cook Sync**: During node evaluation (`Cook`), operators sample the latest atomic state or drain incoming event queues without blocking the main engine thread.

### 2. MIDI Subsystem (`MidiManager`)
- Implemented with Windows MultiMedia (WinMM) high-resolution multimedia timers and callbacks (`midiInOpen`, `midiInStart`, `midiOutOpen`, `midiOutShortMsg`).
- Built-in virtual loopback simulation mode for automated headless CI/CD testing.
- Channel state matrix ($16 \times 128$ CC, $16 \times 128$ Notes, 16 PitchBend, 16 Aftertouch) providing instantaneous lookup and pulse generation.

### 3. Real-Time OSC Channel Mapping (`OSCInChanOp`, `OSCOutChanOp`)
- Complements Phase 8's tabular `OSCInOp` (DataOp) by offering a high-performance **ChanOp** pipeline.
- Binary OSC decoder unpacks OSC typetags (`f`, `i`, `s`, `d`, `b`) and dynamically binds address paths (e.g. `/fader1` $\rightarrow$ `fader1`, `/pos` $[x, y, z]$ $\rightarrow$ `pos_1, pos_2, pos_3`) directly into contiguous SIMD `ChannelBuffer` storage.
- Multi-target UDP broadcaster for outgoing channel bundles.

### 4. Serial Engine (`SerialPort`, `SerialDataOp`)
- Asynchronous Win32 Overlapped I/O (`CreateFileA`, `SetCommState`, `SetCommTimeouts`, `ReadFile`, `WriteFile`, `GetOverlappedResult`).
- Line-buffered ASCII streaming mode (newline `\n` or `\r\n` delimited sensor strings) and raw circular byte buffering.
- Auto-reconnection daemon with configurable baud rates ($9600$ to $921600$).

### 5. DMX512 & Art-Net 4 Protocol (`ArtNetEngine`, `DMXInChanOp`, `DMXOutChanOp`)
- Native implementation of Art-Net 4 protocol over UDP Port 6454.
- ArtDmx packet encoding/decoding: 18-byte header (`"Art-Net\0"`, OpCode `0x5000`, Version `14`, Sequence, Physical, SubNet/Universe, Length 512) followed by 512 bytes of DMX payload.
- Transmits/receives up to 32,768 universes with normalized $[0.0..1.0]$ float $\leftrightarrow$ $[0..255]$ byte mapping.

### 6. Interactive Input Subsystem (`InputManager`, `MouseInChanOp`, `KeyboardInChanOp`)
- Multi-mode coordinate space: Normalized $[0..1]$ window coordinates, viewport pixel coordinates, delta motion $(dx, dy)$, and scroll wheel.
- Win32 raw input and `GetAsyncKeyState` polling for background global hotkeys and interactive multi-projector kiosk installations.

## Consequences
- Zero third-party runtime bloat: Uses native Windows networking (`ws2_32.lib`), multimedia (`winmm.lib`), and standard C++23 multithreading.
- Sub-millisecond latency for stage control with 0 CPU stalls in graph evaluation.
- Full operator parity with TD-class interactive control systems.
