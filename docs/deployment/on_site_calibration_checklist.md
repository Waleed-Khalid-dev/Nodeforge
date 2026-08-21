# Neo Realms — On-Site Show Setup & Calibration Checklist

**Venue:** ___________________________  
**Lead Engineer:** ____________________  
**Date:** _____________________________  
**Show Profile:** [ ] Façade Mapping  [ ] 3D Floor Plan  [ ] Stage AV  [ ] DMX Show Control  

---

## 1. Pre-Flight Hardware Checks

- [ ] **Power & UPS:** Show machine connected to dedicated online Sine-Wave UPS (min 1500VA).
- [ ] **Display Cables:** Certified optical DisplayPort 1.4 / HDMI 2.1 cables securely locked to GPU and projectors.
- [ ] **Network Switch:** Dedicated Gigabit switch connected; Art-Net (Port 6454) and OSC (Port 8000) subnets verified.
- [ ] **EDID Emulation:** NVIDIA Control Panel EDID cloned and locked across all active outputs.
- [ ] **Windows Settings:** Sleep = Never, Screen Turn Off = Never, Windows Notifications = Disabled.

---

## 2. Projector Alignment & Geometry Calibration

- [ ] **Physical Focus & Zoom:** Lens shift, optical focus, and keystone physically zeroed on projector bodies.
- [ ] **ProjectorOutTexOp Routing:** Verify Screen 0 maps to Physical Display 1, Screen 1 maps to Physical Display 2.
- [ ] **Calibration Pattern:** Activate `show_grid = true` on all `WarpBlendTexOp` nodes.
- [ ] **Bezier Patch Warping:** Adjust corner and interior Bezier control vertices in the live Viewer to match architectural features.
- [ ] **Softedge Overlap:** Tune `blend_width` (typically 10-18%) and `blend_gamma` (2.2) until overlap seam is invisible.
- [ ] **Save State:** Press `Ctrl + S` to save active warp parameters into `.nfp` project.

---

## 3. Protocol & Show Control Validation

- [ ] **OSC Stream:** Sensor packets arriving cleanly on port 8000 without packet drop.
- [ ] **MIDI Faders:** Physical MIDI CC sliders respond in Parameter Inspector.
- [ ] **Art-Net DMX:** Lighting fixtures respond to cue triggers with zero frame latency.
- [ ] **Audio Interface:** ASIO / WASAPI output latency tuned to < 10ms.

---

## 4. Performance Soak & Final Sign-Off

- [ ] **Framerate Stability:** Stable 60.0 FPS verified on `PerformanceHUD` (`F3`).
- [ ] **GPU Cook Budget:** Average node cook time < 8.0 ms.
- [ ] **TexturePool Memory:** Zero growth in GPU allocated VRAM over 30-minute test run.
- [ ] **Kiosk Auto-Start:** Reboot machine and verify `nodeforge_player.exe` auto-starts into fullscreen show mode within 45 seconds.

**Sign-off Signature:** _____________________________ **Time:** _____________
