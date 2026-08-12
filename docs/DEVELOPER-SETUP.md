# NodeForge Developer Setup Guide

Follow this guide strictly to set up your Windows machine for NodeForge development. If you miss a step (especially the `VCPKG_ROOT` environment variable), the build will fail.

---

## 1. Core Build Tools (Mandatory for Phase 1)

These tools are required to compile the C++ codebase.

### Visual Studio 2022
1. Download **Visual Studio 2022 Community or Professional**.
2. During installation, select the **"Desktop development with C++"** workload.
3. Ensure the **MSVC v143 - VS 2022 C++ x64/x86 build tools** and **Windows 11 SDK** options are checked on the right side.

### CMake
1. Download CMake (v3.28 or newer) from [cmake.org](https://cmake.org/download/).
2. Run the installer. 
3. **Important:** Select the option to **Add CMake to the system PATH for all users**.

### Ninja Build System
1. Download the Windows release zip from [ninja-build.org](https://ninja-build.org/).
2. Extract the `ninja.exe` file.
3. Move `ninja.exe` to a permanent folder (e.g., `C:\Program Files\Ninja\`).
4. Open the Windows Start Menu, type "Environment Variables", and add that folder path to your **System PATH**.

### vcpkg (Package Manager)
NodeForge uses `vcpkg` to automatically handle 20+ dependencies.
1. Open PowerShell and navigate to where you want to install vcpkg (e.g., `C:\dev\` or `D:\tools\`).
2. Clone the repo:
   ```powershell
   git clone https://github.com/microsoft/vcpkg.git
   ```
3. Run the bootstrap script:
   ```powershell
   cd vcpkg
   .\bootstrap-vcpkg.bat
   ```
4. **CRITICAL:** Set a Windows Environment Variable named `VCPKG_ROOT`. 
   - Open Start Menu → search "Environment Variables" → "Edit the system environment variables".
   - Under "User variables" or "System variables", click **New**.
   - Variable name: `VCPKG_ROOT`
   - Variable value: The full path to your vcpkg folder (e.g., `C:\dev\vcpkg`).

> *Note: You DO NOT need to manually download libraries like GLFW, Dear ImGui, FFmpeg, or Vulkan. vcpkg will automatically download and build them the first time you compile NodeForge.*

---

## 2. Manual SDK Downloads

These libraries require manual downloads due to licensing or registration. You will need them as the project phases unlock.

| SDK / Tool | Why we need it | Where to get it | Notes |
|------------|----------------|-----------------|-------|
| **Python 3.11 Embeddable** | For embedded Python scripting (Phase 3) | [python.org/downloads/windows](https://www.python.org/downloads/windows/) | Download the "Windows embeddable package (64-bit)" for 3.11.x (Do not use 3.12 or 3.13). Extract it into `NodeForge/third_party/python/` when we reach Phase 3. |
| **NDI SDK v6.3.2** | Sending/receiving live video over LAN (Phase 10) | [ndi.video/developers/sdk/](https://ndi.video/developers/sdk/) | Registration required (free). Run the Windows installer. |
| **Spout2 SDK** | Sharing GPU textures with other apps (Phase 10) | [GitHub - Spout2](https://github.com/leadedge/Spout2) | Download the repository or release zip. |
| **Intel RealSense SDK** | Depth camera for gesture tracking (Phase 11) | [GitHub - librealsense](https://github.com/IntelRealSense/librealsense/releases) | Download the `Intel.RealSense.SDK-WIN10` installer. |
| **DeckLink SDK** | Capture card video input (Phase 10) | [Blackmagic Developer](https://www.blackmagicdesign.com/developer/) | Download the "Desktop Video SDK" (free). |

---

## 3. How to Build the Project

Once the Core Build Tools (Section 1) are installed, you can compile NodeForge.

1. Open PowerShell.
2. Navigate to the NodeForge project folder (where `CMakeLists.txt` is located).
3. Generate the build files:
   ```powershell
   cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
   ```
   *(Note: The very first time you run this, it will take a long time because vcpkg is downloading and compiling all third-party libraries from source).*
4. Compile the code:
   ```powershell
   cmake --build build
   ```
5. Run the executable:
   ```powershell
   .\build\bin\nodeforge.exe
   ```

---

## 4. IDE Recommendations

For writing C++ code, we recommend using one of the following:

1. **Visual Studio 2022:** (Just open the local folder, it supports CMake natively).
2. **VS Code:** Install the "C/C++" and "CMake Tools" extensions. Point the CMake tools to your `VCPKG_ROOT` toolchain file.
3. **CLion:** Natively supports CMake and vcpkg.
