# ADR-0002: File Formats for Projects and Components

**Date:** 2026-08-11  
**Status:** Accepted  

## Context

NodeForge needs a way to save and load projects (graphs, parameters, scripts, layouts) and reusable components. We need to decide on the file format and structure for these saves.

## Decision

We will use JSON for serialization of all project data in Version 1.

*   **`.nfp` (NodeForge Project):** The main project file. It will contain the entire graph state, window layouts, parameters, and embedded Python scripts for a show.
*   **`.nfc` (NodeForge Component):** A reusable subgraph. Used for saving individual operators or networks that can be dragged and dropped into other `.nfp` projects.

We will use the `nlohmann/json` library for serialization and deserialization in C++.

## Consequences

*   **Pros:** JSON is human-readable, diff-friendly (good for git version control of projects), and easy to parse in C++ and Python.
*   **Cons:** JSON can become bloated for very large projects. If this becomes an issue in Phase 6+, we will evaluate moving to a binary format (like BSON or MessagePack) or adding a compression layer, but JSON will remain the baseline.
