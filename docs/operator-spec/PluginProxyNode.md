# Operator Specification: PluginProxyNode

## 1. Overview & Classification
- **Name:** `PluginProxyNode` (and dynamic aliases registered by plugins)
- **Family:** Multi-family (`TexOp`, `ChanOp`, `DataOp`, `GeomOp`)
- **Version:** 1.0.0
- **Category:** Custom / Plugin
- **Description:** Dynamically instantiated proxy operator bridging NodeForge's graph execution engine to external native dynamic libraries (`.dll`) via the versioned `nf_plugin_abi.h` C ABI.

---

## 2. Pins (Inputs & Outputs)
Dynamic based on the plugin operator definition returned during `NF_RegisterOperators()`.

### Example Pin Definitions
| Pin Name | Direction | Pin Type | Description |
|---|---|---|---|
| `Input 0..N` | Input | `Texture2D` / `ChannelBuffer` / `DataTable` / `Geometry` | Typed input streams passed to plugin |
| `Output 0..N` | Output | `Texture2D` / `ChannelBuffer` / `DataTable` / `Geometry` | Computed output streams produced by plugin |

---

## 3. Parameters
Dynamic based on the parameter definitions registered by the plugin operator. Supported parameter types:
- `Float` (with min, max, step, default)
- `Int` (with min, max, default)
- `Bool` (checkbox)
- `String` (text input / file path)
- `RGBA` (color picker)
- `Menu` (dropdown selection)
- `Pulse` (action trigger button)

---

## 4. Cook Semantics & Lifecycle
1. **Creation**: The host engine creates a `PluginNodeProxy` instance which calls the plugin's `createInstance()` vtable function to allocate plugin state.
2. **Parameter Sync**: Prior to cooking, changed parameters in NodeForge's `ParameterGroup` are written into the C ABI parameter buffer.
3. **Inputs Marshaling**:
   - `TexOp`: Input `VkImageView`, `VkImage`, `VkFormat`, dimensions, and descriptor sets are populated into `NF_VulkanTextureDescriptor`.
   - `ChanOp`: Input channel float pointers, channel count, sample count, and sample rate are populated into `NF_ChannelBufferDescriptor`.
   - `DataOp`: Input table cell pointers and dimensions are populated into `NF_DataTableDescriptor`.
4. **Cook Call**: Host invokes `cook(instance, &cookContext, &inputs, &outputs)`.
5. **Outputs Marshaling**: The plugin writes to output buffers or GPU render targets, which the proxy forwards to output pins.
6. **Destruction**: When the node is deleted or the plugin is unloaded, `destroyInstance()` is invoked to free plugin memory.

---

## 5. Error Handling & Safety
- **Exception Isolation**: All vtable invocations are wrapped in structured C++ exception barriers. Exceptions in third-party code log error messages with node names and flag the node as errored without crashing the host.
- **ABI Validation**: On load, `NF_PLUGIN_ABI_VERSION` is validated. Mismatched plugins are rejected with clear error codes.

---

## 6. Test Plan
- `plugin_abi_test.cpp`: Struct layout, alignment, and version handshake.
- `plugin_manager_test.cpp`: Load, cook, parameter sync, output verification, hot-reload, and safe unload.
