# Project Plan: Phase 2 — Graph Runtime (Heart of the Product)

**Active Phase:** Phase 2  
**Target:** In-memory DAG Graph with Cook Engine, Dirty Propagation, 4 Starter Operators, and Test Suite  
**Owner:** Waleed Khalid / Neo Realms  

---

## 🎯 Phase 2 Goals & Overview

Implement the core dataflow evaluation engine for NodeForge. In Phase 2, the graph operates purely in-memory (no UI canvas yet — that arrives in Phase 4) with deterministic topological cooking, dirty flag propagation, zero unnecessary re-cooks, cycle prevention, and unit/benchmark verification.

---

## 📐 Architectural Decisions (Derived from /grill-me Alignment)

1. **Evaluation Model (ADR-0004):** Hybrid Pull-on-Demand with Push Dirty Invalidation.
   - Parameter edits or input changes immediately push `m_isDirty = true` to all downstream dependents.
   - Sinks / root nodes trigger a pull-based `Cook(CookContext& context)`.
   - Each node memoizes its output and checks `lastCookFrame == context.frameIndex` to ensure each node cooks **at most once per frame** (even in complex diamond topologies).
2. **Data Payloads (`nf::PinValue`):** Type-safe `std::variant` supporting:
   - `std::monostate` (empty/unconnected)
   - `float`, `int32_t`, `bool`, `std::string`, `glm::vec2`, `glm::vec3`, `glm::vec4`
   - `nf::ChannelBuffer` (multi-channel audio/data samples with sample rate and channel names)
   - `std::shared_ptr<gpu::Texture2D>` (GPU texture payload for TexOps)
3. **Cycle Rejection:**
   - Fast reachability check on `Graph::Connect(outputPin, inputPin)` returning `nf::Result::CycleDetected` error before wires are created.
   - Kahn's topological sort runtime guard during graph execution.
4. **Registry Architecture:**
   - Centralized `nf::RegisterCoreNodes(nf::NodeRegistry&)` function to avoid static initialization order issues.
5. **Agent Rules Compliance:**
   - `docs/adr/ADR-0004-graph-runtime.md` written before code.
   - 4 operator specs written in `docs/operator-spec/` before implementation.

---

## 🗂️ Task Breakdown

### Phase 2.1: Architecture & Operator Specifications (Documentation First)
- [ ] `docs/adr/ADR-0004-graph-runtime.md`
- [ ] `docs/operator-spec/ConstantTexOp.md`
- [ ] `docs/operator-spec/TransformTexOp.md`
- [ ] `docs/operator-spec/ConstantChanOp.md`
- [ ] `docs/operator-spec/MathChanOp.md`

### Phase 2.2: Core Graph Data Structures (`src/graph/`)
- [ ] `src/graph/PinValue.h` & `src/graph/PinValue.cpp`: `PinValue` variant, `ChannelBuffer`, helper accessors.
- [ ] `src/graph/Pin.h` & `src/graph/Pin.cpp`: Input/Output pins, direction, family type, default values, connection queries.
- [ ] `src/graph/Wire.h` & `src/graph/Wire.cpp`: Directed connection between two pins with ID generation.
- [ ] `src/graph/CookContext.h`: Frame index, time, delta time, GPU device pointer.
- [ ] `src/graph/Node.h` & `src/graph/Node.cpp`: Base class with parameter dictionary, pin vectors, `Cook()` virtual method, dirty propagation logic.
- [ ] `src/graph/Graph.h` & `src/graph/Graph.cpp`: Node container, wire connection/disconnection, cycle detection, topological sort, graph-level cook triggers.

### Phase 2.3: Node Registry & Factory (`src/graph/`)
- [ ] `src/graph/NodeTypeInfo.h`: Operator metadata (name, family, category, pin definitions, factory function).
- [ ] `src/graph/NodeRegistry.h` & `src/graph/NodeRegistry.cpp`: Operator registration map and factory instantiation.
- [ ] `src/graph/CoreNodes.h` & `src/graph/CoreNodes.cpp`: Explicit registration function `RegisterCoreNodes()`.

### Phase 2.4: Starter Operators (`src/operators/`)
- [ ] `src/operators/chan/ConstantChanOp.h` & `.cpp`: Produces constant scalar/vector channel values.
- [ ] `src/operators/chan/MathChanOp.h` & `.cpp`: Performs add, multiply, sine, clamp, etc. on input channels.
- [ ] `src/operators/tex/ConstantTexOp.h` & `.cpp`: Generates solid color 2D GPU textures via Vulkan/VMA.
- [ ] `src/operators/tex/TransformTexOp.h` & `.cpp`: Applies 2D translation/rotation/scale on GPU texture streams.

### Phase 2.5: Build System Updates & CMake
- [ ] Update `CMakeLists.txt` to include `src/graph/*.cpp` and `src/operators/*/*.cpp`.
- [ ] Link `nodeforge` and `nodeforge_tests`.

### Phase 2.6: Verification & Test Harness (`tests/`)
- [ ] `tests/unit/graph_dag_test.cpp`:
  - Linear 100-node graph cook order test.
  - Diamond dependency graph (`A -> B, A -> C; B -> D, C -> D`) ensuring node `A` cooks **exactly once**.
  - Cycle detection & connection rejection test.
  - Dirty propagation and memoization test.
- [ ] `tests/unit/operators_test.cpp`:
  - `ConstantChanOp` & `MathChanOp` calculation verification.
  - `ConstantTexOp` & `TransformTexOp` texture generation test.
- [ ] `tests/benchmark/graph_benchmark.cpp`:
  - 1,000-node dirty propagation latency benchmark (< 1 ms requirement).

---

## 🏁 Definition of Done (Roadmap Verification)

- [ ] 100-node linear graph cooks in correct order
- [ ] Diamond graph (`A → B, A → C, B → D, C → D`) cooks `A` once, `B` and `C` once, `D` once
- [ ] Cycle rejected with clean error (`Result::CycleDetected`)
- [ ] Benchmark: 1,000 nodes dirty propagation < 1 ms
- [ ] All unit and benchmark tests pass via `ctest` / `nodeforge_tests.exe`
- [ ] `STATUS.md` and `02-BUILD-ROADMAP-A-to-Z.md` updated and pushed to remote

---

## 🚫 Forbidden in Phase 2
- Visual node editor canvas UI (reserved for Phase 4)
- Python runtime embedding in nodes (reserved for Phase 6)
- File persistence `.nodeforge` save/load (reserved for Phase 8)
