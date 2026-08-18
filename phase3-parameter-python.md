# Phase 3 Implementation Plan — Parameter System + Expressions + Python Embed

## 1. Objectives & Scope
Phase 3 builds the scripting and expression superpower of NodeForge (TouchDesigner-equivalent parameter & Python model).
- Embed CPython 3 via `pybind11`.
- Implement a structured, typed `Parameter` system supporting static literals and dynamic Python expressions.
- Expose the complete `nodeforge` Python API (`op()`, `create_node()`, `connect()`, `disconnect()`, `par`).
- Ensure robust GIL management and crash-resilient exception handling.
- Verify through comprehensive unit and integration tests.

---

## 2. Architecture Decisions (Confirmed via /grill-me)
1. **Dual-Mode Parameter (`ParamMode`):** Parameters support `Constant` and `Expression` modes. Expression mode dynamically compiles and evaluates Python expressions during cook.
2. **Python Expression Engine:** All parameter expressions (e.g., `math.sin(me.time * 2.0)`, `op('Const1').par.values.x + 5`) execute in the embedded Python runtime.
3. **Dedicated `PythonEngine` Singleton:** RAII initialization, frame-level GIL guard, exception safety wrapping all Python execution.
4. **`nodeforge` Module Bindings:** `op()`, `create_node()`, `connect()`, `disconnect()`, `node.par.<name>`, `node.get_param()`, `node.set_param()`, `node.cook()`, `node.is_dirty`.
5. **Structured Parameter Class:** Includes limits (min/max), default values, type descriptors, and change observers that mark downstream nodes dirty.

---

## 3. Work Breakdown Structure

### Component 1: Architecture & ADR
- `docs/adr/ADR-0005-parameter-system-and-python.md`: Document parameter model, expression evaluation, Python embedding, and GIL safety policy.

### Component 2: Dependencies (`vcpkg.json` & `CMakeLists.txt`)
- Add `pybind11` and `python3` to `vcpkg.json` and `CMakeLists.txt`.
- Configure include directories and library linking for `nodeforge_core` and `nodeforge_tests`.

### Component 3: Parameter Subsystem (`src/param/`)
- `src/param/ParamType.h`: Parameter data types and metadata descriptor.
- `src/param/Parameter.h` & `src/param/Parameter.cpp`: Parameter class with `ParamMode`, value storage, expression string, evaluated value, change events, and dirty invalidation.
- `src/param/ParameterGroup.h` & `src/param/ParameterGroup.cpp`: Container for node parameters with name/index lookups and serialization helpers.

### Component 4: Python Engine & Bindings (`src/python/`)
- `src/python/PythonEngine.h` & `src/python/PythonEngine.cpp`: CPython interpreter lifecycle, GIL management, expression evaluation helper, and error logging.
- `src/python/PyNodeForge.h` & `src/python/PyNodeForge.cpp`: `pybind11` module definitions for `nodeforge`, `Node`, `Pin`, `Wire`, `Graph`, and `Parameter`.

### Component 5: Graph & Operator Integration
- Update `src/graph/Node.h` & `src/graph/Node.cpp` to use `ParameterGroup`.
- Update `ConstantChanOp`, `MathChanOp`, `ConstantTexOp`, and `TransformTexOp` to declare structured parameters.
- Update `GraphSerializer` to serialize parameter modes and expression strings.

### Component 6: Tests & Verification
- `tests/unit/parameter_test.cpp`: Test parameter types, limits, expression evaluation, and dirty propagation.
- `tests/unit/python_embed_test.cpp`: Test Python interpreter initialization, `nodeforge` module bindings, creating/connecting nodes via Python, mutating parameters, and verifying exception safety.

---

## 4. Definition of Done (Roadmap Section 3)
- [ ] Python REPL or script file can mutate graph safely (create nodes, connect pins, set parameters).
- [ ] Expression on a parameter evaluates each cook when dirty.
- [ ] Crashes or syntax errors in Python do not bring down process without log.
- [ ] 100% of unit tests pass under `nodeforge_tests.exe`.
