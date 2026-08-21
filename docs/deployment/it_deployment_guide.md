# NodeForge Enterprise IT Operations & Field Deployment Guide

**Target Audience:** Venue IT Engineers, Systems Administrators, Field Technicians  
**Platform Target:** Windows 10/11 Enterprise 64-bit  
**Minimum Show Hardware:** NVIDIA RTX 3060 12GB+ / 16GB DDR4 RAM / Core i7 or Ryzen 7 / PCIe NVMe SSD

---

## 1. Runtime Prerequisites & Air-Gapped Staging

Before deploying NodeForge to an isolated or air-gapped installation PC, ensure the following runtime packages are installed:

1. **GPU Driver:** NVIDIA Studio Driver (version 550.00+ or latest stable LTS branch).
2. **Vulkan 1.3 Runtime:** Included with modern NVIDIA drivers. Verify using `vulkaninfo.exe`.
3. **Visual C++ Redistributable:** Microsoft Visual C++ 2015-2022 Redistributable (x64) version 14.38+.
4. **Embedded Python Runtime:** CPython 3.11 x64 embedded binaries (bundled inside `bin/python311.dll`).
5. **DirectX / Spout2 / NDI Runtime:** NDI 5+ Runtime Redistributable (if network video streaming is required).

---

## 2. Windows OS Configuration & Optimization

To ensure zero dropped frames and uninterrupted 24/7 continuous show operation:

1. **Power Plan:** Set Windows Power Plan to **High Performance** / **Ultimate Performance**.
2. **Disable Sleep & Hibernation:** Set Screen Turn Off = `Never`, Sleep = `Never`, PCIe Link State Power Management = `Off`.
3. **Windows Update Policy:** Pause or configure Windows Update via Group Policy (`gpedit.msc`) to prevent unannounced show reboots.
4. **Firewall & Port Rules:**
   - **Art-Net 4 DMX512:** Allow UDP incoming/outgoing on Port `6454`.
   - **Open Sound Control (OSC):** Allow UDP incoming/outgoing on Ports `8000`, `9000`.
   - **NDI Video:** Allow TCP/UDP Ports `5960-5969`, `6960-6969`, `7960-7969`.
5. **Multi-Display & EDID Locking:**
   - In NVIDIA Control Panel -> `Workstation` -> `View system topology`, lock the EDID on all projector DisplayPort/HDMI outputs to prevent Windows display rearrangement if a projector is power-cycled.

---

## 3. Autonomous Kiosk Mode Deployment (`nodeforge_player.exe`)

NodeForge includes a dedicated, lightweight standalone player executable (`nodeforge_player.exe`) stripped of editor UI overhead for field installations.

### Command-Line Arguments

```cmd
nodeforge_player.exe --project "D:\Shows\facade_mapping.nfp" --fullscreen --borderless --log "D:\Shows\player.log"
```

| Argument | Description |
|----------|-------------|
| `--project <path>` | Absolute or relative path to the `.nfp` project to execute on startup |
| `--fullscreen` | Launches all `ProjectorOutTexOp` presentation windows in borderless fullscreen |
| `--fps <target>` | Overrides timeline framerate target (e.g. `60.0` or `120.0`) |
| `--log <path>` | Directs spdlog execution logs to a persistent file on disk |

### Windows Startup Automation (Task Scheduler)

1. Open `Task Scheduler` -> `Create Task`.
2. **General:** Check `Run with highest privileges`.
3. **Triggers:** `At log on` of the show operator account.
4. **Actions:** `Start a program` -> Point to `nodeforge_player.exe` with project arguments.
5. **Conditions:** Uncheck `Start the task only if the computer is on AC power`.
6. **Settings:** Check `If the task fails, restart every 1 minute` (Up to 3 attempts).

---

## 4. Crash Recovery & Diagnostic Analysis

In the unlikely event of an unhandled hardware exception:

1. **SEH Crash Dump:** NodeForge's Win32 Structured Exception Handler (SEH) writes a diagnostic stack trace to `nodeforge_crash.log`.
2. **Emergency State Snapshot:** An emergency snapshot of the active graph is saved to `<project_name>.nfp.crash`.
3. **Recovery:** To resume operation after unexpected power loss, load `<project_name>.nfp.crash` or inspect `nodeforge_crash.log` for faulting modules (e.g. third-party plugin DLLs or corrupted media files).
