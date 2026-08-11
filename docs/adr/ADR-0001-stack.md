# ADR-0001: Core Technology Stack

**Date:** 2026-08-11  
**Status:** Accepted  

## Context

NodeForge requires a real-time capable architecture to process nodes, execute GPU shaders, display a high-performance UI, and allow Python scripting. We need to choose the foundational stack that will last the lifetime of the product.

## Decision

We will use the following core technologies:

*   **Language:** C++23. Provides the best mix of high-performance real-time capabilities and modern language features.
*   **Build System:** CMake 3.28+ with Ninja, and `vcpkg` for dependency management.
*   **Primary GPU API:** Vulkan 1.3 (via `vk-bootstrap`). It provides modern, explicit control over the GPU, which is crucial for deterministic cook loops and multi-queue rendering.
*   **Shader Language:** GLSL compiled to SPIR-V via `shaderc/glslang`. Familiar to technical artists.
*   **UI Framework:** Dear ImGui (docking branch) + `imgui-node-editor`. This combination allows for extremely fast iteration of complex node-based interfaces.
*   **Scripting:** CPython 3.11 embedded, bound via `pybind11`. Matches the industry standard established by tools like TouchDesigner.

## Consequences

*   **Pros:** Highly performant, modern, explicit control over hardware, massive ecosystem support via `vcpkg`.
*   **Cons:** Vulkan has a steeper learning curve than OpenGL (mitigated by `vk-bootstrap`). Dear ImGui requires custom styling to look like a premium application.
