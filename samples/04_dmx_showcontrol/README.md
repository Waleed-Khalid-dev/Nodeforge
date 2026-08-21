# Neo Realms — Art-Net DMX512 & Show Control System

**Project:** `samples/04_dmx_showcontrol/dmx_showcontrol.nfp`  
**Target Hardware:** Art-Net DMX Node (e.g., Enttec ODE, DMXKing) / Microcontroller (Arduino/ESP32 via USB Serial)  
**Primary Engine Features:** `TimeChanOp`, `LFOChanOp`, `MathChanOp`, `DMXOutChanOp`, `SerialDataOp`, `TableDataOp`, `KeyboardInChanOp`.

---

## 1. Overview

This project provides a robust show control and automated lighting engine. It converts continuous math and LFO animation waveforms into standard 512-channel DMX byte values (`0-255`) and broadcasts them over UDP Ethernet via Art-Net 4 packets on Port 6454. Microcontroller sensors (e.g. physical stage buttons, proximity sensors) feed telemetry into `SerialDataOp` at 115200 baud, while hotkeys on the keyboard allow live operators to trigger scene and blackout transitions.

```
       ┌─────────────────────┐
       │   timeline_clock    │ (TimeChanOp)
       └──────────┬──────────┘
                  │
       ┌──────────▼──────────┐
       │     chase_lfo       │ (LFOChanOp Sine Waveform)
       └──────────┬──────────┘
                  │
       ┌──────────▼──────────┐
       │   dmx_byte_scale    │ (MathChanOp: Remap 0..1 to 0..255)
       └──────────┬──────────┘
                  │
       ┌──────────▼──────────┐
       │   artnet_dmx_out    │ (DMXOutChanOp: Art-Net 4 UDP Port 6454)
       └─────────────────────┘
                  │
                  ▼ [ Ethernet Network to DMX Dimmer / LED Fixtures ]

       ┌─────────────────────┐      ┌─────────────────────┐
       │   arduino_serial    │      │      cue_keys       │
       │ (SerialDataOp COM3) │      │ (KeyboardIn: 1-4)   │
       └──────────┬──────────┘      └──────────┬──────────┘
                  │                            │
                  └─────────────┬──────────────┘
                                ▼
                     ┌─────────────────────┐
                     │      cue_table      │ (Lighting Cues & State)
                     └─────────────────────┘
```

---

## 2. Art-Net Configuration

- **Protocol:** Art-Net 4 (DMX512-A)
- **Transport:** UDP IPv4
- **Port:** `6454`
- **Destination:** `127.0.0.1` (Local loopback or broadcast IP `192.168.1.255`)
- **Universe:** 0 (Subnet: 0, Net: 0)
- **Frame Rate:** 60 packets / second hardware refresh
