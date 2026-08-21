# Workshop Lab 4: Protocols, Gestures & Live Show Control

**Module:** 5 | **Objective:** Build an interactive show control network with OSC gesture input, MIDI hardware faders, and Art-Net DMX lighting.

---

## 1. Step-by-Step Build Instructions

1. **Receive OSC Sensor Telemetry:**
   - Create an `OSCInChanOp`. Set `port` = `8000`, `address_pattern` = `/gesture/*`.
2. **Apply Jitter Filter:**
   - Create a `FilterChanOp`. Wire `oscin1.output` -> `filter1.input`. Set `filter_width` = `0.1` seconds.
3. **Capture MIDI Controller Hardware:**
   - Create a `MIDIInChanOp`. Set `device_index` = `0`, `channel` = `1`.
4. **Remap Channel to DMX Range:**
   - Create a `MathChanOp`. Wire `filter1.output` -> `math1.input`.
   - Set `operation` = `Multiply`, `scalar` = `255.0`, `clamp_min` = `0.0`, `clamp_max` = `255.0`.
5. **Broadcast Art-Net DMX512:**
   - Create a `DMXOutChanOp`. Wire `math1.output` -> `dmxout1.input`.
   - Set `ip_address` = `127.0.0.1` (or local lighting network broadcast `192.168.1.255`), `port` = `6454`, `universe` = `0`.
6. **Add Emergency Blackout Key:**
   - Create a `KeyboardInChanOp`. Set `keys` = `space 1 2 3`.
   - Wire keypresses into a `ScriptDataOp` to toggle lighting state variables on cue pulses!
