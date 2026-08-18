# ADR-0005: Parameter System, Expression Evaluation, and Python Embedding

## Status
Accepted (Phase 3)

## Context
NodeForge requires a flexible parameter system that allows operator attributes (numbers, strings, vectors, colors, filepaths) to be driven either statically by direct user input or dynamically by expressions and scripts. In TouchDesigner, this duality (Constants vs Expressions) powers interactive installations, sensor mappings, and mathematical animations.

We also need an embedded scripting engine that provides complete control over the graph (creating nodes, wiring pins, inspecting state, and driving parameter values) while preventing user scripts or syntax errors from crashing the host process.

## Decisions

### 1. Dual-Mode Parameter Model
- Every `Parameter` belongs to a `ParameterGroup` on a `Node`.
- A parameter has a `ParamMode`:
  - `Constant`: The parameter evaluates to its static `PinValue` (e.g., float, int, vec2, vec4, string).
  - `Expression`: The parameter evaluates an expression string dynamically within the embedded Python runtime each frame the node is cooked.
- Modifying a parameter's static value or expression string triggers `Node::MarkDirty()`, propagating downstream to invalidate dependent cook caches.

### 2. Embedded CPython 3 via pybind11
- CPython 3 is embedded using `pybind11::embed`.
- A dedicated `PythonEngine` singleton manages the interpreter lifecycle (`py::scoped_interpreter`), provides GIL locking guards, and injects runtime context:
  - `me`: References the current node evaluating the expression.
  - `op(name_or_id)`: Lookup function for any node in the active graph.
  - `time`: Absolute time in seconds, delta time, and frame number.
  - Standard Python math, numpy (if available), and system libraries.
- All Python invocations (expressions and scripts) are wrapped in `try/catch (py::error_already_set&)` exception guards. Errors are logged with tracebacks and set a warning/error flag on the node without terminating the engine.

### 3. `nodeforge` Python Module Bindings
The embedded interpreter exposes a native C++ module `nodeforge` with:
- `nodeforge.op(name_or_id)`: Returns a `Node` object.
- `nodeforge.create_node(type_name, name)`: Spawns and inserts a node into the active graph.
- `nodeforge.connect(from_op, from_pin, to_op, to_pin)`: Wires two nodes with cycle checks.
- `nodeforge.disconnect(to_op, to_pin)`: Unwires an input pin.
- `nodeforge.get_nodes()`: Lists all nodes in the graph.
- `Node.par.<name>` / `Node.get_param(name)` / `Node.set_param(name, value)`: Inspects and updates parameters.
- `Node.cook()` / `Node.is_dirty`: Triggers and inspects cook state.

### 4. Parameter Metadata & Limits
- Each parameter defines metadata: `ParamType`, `label`, `defaultValue`, `minLimit`, `maxLimit`, `page/category`, and `isClamped`.
- Numerical parameters clamp values to their defined limits if clamping is enabled.

## Consequences
- **Positive:** Intuitive TD-like workflow where any parameter can be scripted; complete Python automation of graph generation; zero crash on Python script errors.
- **Trade-offs:** Python evaluation has slight overhead compared to pure C++ arithmetic, mitigated by cook memoization (expressions only evaluate when the node is dirty or time-dependent).
