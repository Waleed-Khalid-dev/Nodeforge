# Comp System & Plugin Cheat Sheet (Containers, Hierarchy & Extensibility)

**Family:** `Comp` | **Color:** Gray / Orange | **Features:** Nested Subnetworks, Boundary Pins, Hot-Reload Plugins

---

## 1. Component Hierarchy & Boundary Routing

`ContainerComp` enables modular visual programming by nesting entire sub-graphs inside a single parent node.

- **`InOp`**: Placed inside a `ContainerComp` to receive input data from the outer parent graph.
- **`OutOp`**: Placed inside a `ContainerComp` to expose processed results back to the outer parent graph.
- **`.nfc` File Export:** Right-click any `ContainerComp` -> `Export Component...` to serialize the subnetwork into a reusable `.nfc` component file.

---

## 2. Dynamic Plugin Architecture

NodeForge loads compiled C ABI dynamic link libraries (`.dll`) from the `plugins/` directory automatically.

- **`PluginProxyNode`**: An engine adapter that wraps external C/C++ plugins into native NodeForge operators across all 6 families (TexOp, ChanOp, DataOp, GeomOp).
- **Hot-Reload:** Modifying or replacing a `.dll` on disk triggers an automatic live reload without restarting the NodeForge Studio or Kiosk runner.
