# ADR-0003: Naming Conventions and Operator Families

**Date:** 2026-08-11  
**Status:** Accepted  

## Context

To maintain a clean and understandable codebase, especially when implementing a node-based architecture similar to TouchDesigner, we need strict naming conventions for namespaces, classes, and operator families.

## Decision

We will adhere to the following naming rules:

*   **Global Namespace:** `nf::` (NodeForge). All core engine code lives here.
*   **Macros/Defines:** `NF_` prefix (e.g., `NF_DEBUG`, `NF_VERSION_MAJOR`).
*   **Product Name:** NodeForge (capital N, capital F in marketing/docs; `nodeforge` in binaries).
*   **Operator Families:** Operators will be grouped into distinct families based on the type of data they process, similar to TouchDesigner's approach, but uniquely named:
    *   **TexOp:** Texture Operators (GPU 2D image processing, shaders).
    *   **ChanOp:** Channel Operators (1D signals, audio, animation curves).
    *   **GeomOp:** Geometry Operators (3D meshes, points).
    *   **MatOp:** Material Operators (Shaders applied to GeomOps).
    *   **DataOp:** Data Operators (Tables, JSON, text, databases).
    *   **Comp:** Components (Containers, sub-networks, UI panels).

## Consequences

*   **Pros:** Prevents naming collisions. Familiar structure for users coming from TouchDesigner without infringing on their specific terminology (TOPs, CHOPs, etc.). Codebase remains highly organized.
*   **Cons:** Requires discipline to enforce across all new operators and files.
