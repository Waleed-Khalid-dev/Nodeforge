# ADR-0004: In-Memory DAG Graph Runtime and Cook Architecture

## Status
Accepted (2026-08-18)

## Context
NodeForge is a real-time node-based media engine designed for interactive installations, projection mapping, and generative graphics. The core engine requires a robust, high-performance graph runtime that can evaluate hundreds to thousands of interconnected nodes per frame with minimal overhead (< 1 ms latency for dirty propagation).

We evaluated three evaluation architectures:
1. **Pure Push (Reactive/Dataflow):** Any parameter modification immediately forces evaluation downstream through connected nodes.
2. **Pure Pull (Lazy Evaluation without memoization):** Sinks traverse the entire graph every frame and recompute all upstream dependencies.
3. **Hybrid Pull-on-Demand with Push Invalidation (TouchDesigner Architecture):** Parameter or input modifications push dirty flags downstream to invalidate cached states. Active outputs (viewports, export sinks) pull inputs on demand during the frame render, evaluating dirty nodes at most once per frame.

## Decision
We adopt the **Hybrid Pull-on-Demand with Push Invalidation** architecture for NodeForge:

### 1. Invalidation Invariants
- When a node parameter or input connection changes, `node->MarkDirty()` is invoked.
- `MarkDirty()` sets `m_isDirty = true` on the target node and recursively propagates `MarkDirty()` to all downstream nodes connected to its output pins.
- If a downstream node is already dirty, the propagation short-circuits to eliminate redundant traversals ($O(1)$ amortized for repeated changes).

### 2. Cook Memoization & Frame Counters
- The engine maintains a global monotonic frame index `CookContext::frameIndex`.
- When `node->EnsureCooked(context)` is called:
  - If `!m_isDirty && m_lastCookFrame == context.frameIndex`, it immediately returns `true` (cache hit).
  - It recursively calls `EnsureCooked(context)` on all upstream connected nodes.
  - It calls `node->Cook(context)` to compute new output payloads.
  - Upon success, `m_isDirty` is reset to `false` and `m_lastCookFrame = context.frameIndex`.
- This ensures diamond topologies (`A -> B, A -> C; B -> D, C -> D`) evaluate node `A` **exactly once per frame**.

### 3. Type-Safe Data Payloads (`nf::PinValue`)
- Payloads between pins are represented by `std::variant<std::monostate, float, int32_t, bool, std::string, glm::vec2, glm::vec3, glm::vec4, nf::ChannelBuffer, std::shared_ptr<gpu::Texture2D>>`.
- This avoids dynamic allocations for scalars/vectors and enables zero-copy sharing of GPU textures via ref-counted handles.

### 4. Cycle Detection
- Directed cycles are strictly forbidden in general DAG connections.
- `Graph::Connect(srcPin, dstPin)` runs a cycle reachability check (DFS) and immediately rejects connections that would create a circular dependency.
- Topological sorting runs Kahn's algorithm as a runtime validation guard.

## Consequences
- **Positive:** Deterministic evaluation order, zero redundant computation, ultra-fast dirty propagation (< 1 ms for 1,000 nodes), clean separation between graph logic and rendering.
- **Negative:** Requires explicit feedback nodes (e.g. `FeedbackTexOp` with 1-frame latency buffer) for intentional feedback loops in future phases.
