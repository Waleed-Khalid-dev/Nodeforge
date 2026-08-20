# NodeForge Plugin SDK

Welcome to the **NodeForge Dynamic Operator Plugin SDK**. This SDK enables developers to extend NodeForge with high-performance native C++23 / C dynamic operators (`.dll`) across GPU Texture Operators (`TexOp`), SIMD Channel/Audio Operators (`ChanOp`), and Tabular Data Operators (`DataOp`).

---

## 🚀 Key Features
- **Strict C ABI (`nf_plugin_abi.h`)**: Pure `extern "C"` interface insulated against C++ compiler / runtime ABI mismatches.
- **Modern C++23 Wrappers (`NodeForgePluginSDK.hpp`)**: Object-oriented base classes, type-safe parameters, and automatic exception boundary guards.
- **Multi-Family Support**:
  - `TexOp`: Direct Vulkan 1.3 texture descriptors (`VkDevice`, `VkCommandBuffer`, `VkImage`, `VkImageView`, formats).
  - `ChanOp`: Raw SIMD float arrays with custom channel counts and sample rates.
  - `DataOp`: 2D string grid tables.
- **Hot-Reloading**: Update your DLL on disk and click **Reload** in NodeForge Studio without restarting the editor.

---

## 📦 Building a Plugin with CMake

A minimal `CMakeLists.txt` for your plugin:

```cmake
cmake_minimum_required(VERSION 3.28)
project(MyCustomPlugin VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(MyCustomPlugin SHARED
    src/MyCustomPlugin.cpp
)

target_include_directories(MyCustomPlugin PRIVATE
    ${NODEFORGE_SDK_DIR}/include
)

target_compile_definitions(MyCustomPlugin PRIVATE
    NF_PLUGIN_BUILD_EXPORT
)
```

---

## 🛠️ Quick Example: Custom ChanOp

```cpp
#include <NodeForgePluginSDK.hpp>
#include <cmath>

class HarmonicLFO : public nf::sdk::ChanOpPlugin {
public:
    using ChanOpPlugin::ChanOpPlugin;

    NF_Result Cook(const NF_CookContext& ctx) override {
        float freq = GetFloat("frequency", 1.0f);
        float amp = GetFloat("amplitude", 1.0f);
        
        // Output 1 channel with 100 samples
        // (Populate channel buffer)
        return NF_SUCCESS;
    }
};

static const NF_PinDef s_pins[] = {
    { "Output", NF_PIN_DIR_OUTPUT, NF_PIN_TYPE_CHANNEL }
};

static const NF_ParamDef s_params[] = {
    { "frequency", "Frequency", "Common", NF_PARAM_FLOAT, 1.0f, 0, false, nullptr, 0.1f, 100.0f, 0.1f, nullptr, 0 },
    { "amplitude", "Amplitude", "Common", NF_PARAM_FLOAT, 1.0f, 0, false, nullptr, 0.0f, 10.0f, 0.1f, nullptr, 0 }
};

static const NF_PluginInfo s_pluginInfo = {
    "HarmonicLFOPlugin", "Neo Realms", "Custom Harmonic LFO Generator", "https://neorealms.io",
    1, 0, 0, NF_PLUGIN_ABI_VERSION, 1
};

extern "C" {

NF_PLUGIN_API const NF_PluginInfo* NF_GetPluginInfo(void) {
    return &s_pluginInfo;
}

NF_PLUGIN_API int32_t NF_RegisterOperators(NF_OperatorDef* outOps, uint32_t maxOps) {
    if (!outOps || maxOps < 1) return 0;
    outOps[0].typeName = "HarmonicLFO";
    outOps[0].family = NF_NODE_FAMILY_CHANOP;
    outOps[0].category = "Custom/Oscillators";
    outOps[0].description = "Harmonic LFO Waveform Generator";
    outOps[0].pinDefs = s_pins;
    outOps[0].pinDefCount = 1;
    outOps[0].paramDefs = s_params;
    outOps[0].paramDefCount = 2;
    outOps[0].vtable = nf::sdk::VTableBinder<HarmonicLFO>::MakeVTable();
    return 1;
}

NF_PLUGIN_API void NF_UnloadPlugin(void) {}

}
```

---

## 🔍 Plugin Discovery & Installation
NodeForge automatically discovers plugins from:
1. `<NodeForge_Directory>/plugins/`
2. `%APPDATA%/NodeForge/plugins/`
3. Project root `./plugins/`
4. Paths listed in the `NF_PLUGIN_PATH` environment variable.
