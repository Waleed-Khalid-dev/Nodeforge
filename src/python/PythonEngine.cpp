#include "PythonEngine.h"
#include "PyNodeForge.h"
#include "../graph/Graph.h"
#include "../graph/Node.h"
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <spdlog/spdlog.h>

namespace py = pybind11;

namespace nf {

struct PythonEngine::Impl {
    std::unique_ptr<py::scoped_interpreter> guard;
    py::module_ mathModule;
};

PythonEngine& PythonEngine::Instance() {
    static PythonEngine instance;
    return instance;
}

PythonEngine::~PythonEngine() {
    Shutdown();
}

void PythonEngine::Initialize() {
    if (m_initialized) return;

    try {
        RegisterPyNodeForgeModule();
#ifdef _WIN32
        if (!std::getenv("PYTHONHOME")) {
#ifdef NF_PYTHON_HOME
            std::string homeStr = NF_PYTHON_HOME;
            static std::wstring pyHome(homeStr.begin(), homeStr.end());
            Py_SetPythonHome(pyHome.c_str());
#endif
        }
#endif
        m_impl = std::make_unique<Impl>();
        m_impl->guard = std::make_unique<py::scoped_interpreter>();
        m_impl->mathModule = py::module_::import("math");
        py::module_::import("nodeforge");
        m_initialized = true;
        spdlog::info("Embedded Python 3 interpreter initialized successfully.");
    } catch (const std::exception& e) {
        spdlog::error("Failed to initialize embedded Python: {}", e.what());
        m_initialized = false;
    }
}

void PythonEngine::Shutdown() {
    if (!m_initialized) return;
    try {
        m_impl.reset();
        m_initialized = false;
        spdlog::info("Embedded Python 3 interpreter shutdown.");
    } catch (const std::exception& e) {
        spdlog::error("Error during Python shutdown: {}", e.what());
    }
}

bool PythonEngine::ExecuteString(const std::string& code, std::string* outError) {
    if (!m_initialized) {
        Initialize();
        if (!m_initialized) {
            if (outError) *outError = "Python engine not initialized";
            return false;
        }
    }

    try {
        py::module_ mainMod = py::module_::import("__main__");
        py::dict globals = mainMod.attr("__dict__");
        if (!globals.contains("__builtins__")) {
            globals["__builtins__"] = py::module_::import("builtins");
        }
        py::exec(code, globals, globals);
        return true;
    } catch (const py::error_already_set& e) {
        std::string err = e.what();
        spdlog::error("Python execution error: {}", err);
        if (outError) *outError = err;
        return false;
    } catch (const std::exception& e) {
        std::string err = e.what();
        spdlog::error("Python execution error: {}", err);
        if (outError) *outError = err;
        return false;
    }
}

PinValue PythonEngine::EvaluateExpression(const std::string& expression, Node* meNode, const CookContext& context, std::string* outError) {
    if (expression.empty()) return PinValue{};

    if (!m_initialized) {
        Initialize();
        if (!m_initialized) {
            if (outError) *outError = "Python engine not initialized";
            return PinValue{};
        }
    }

    try {
        py::module_ mainMod = py::module_::import("__main__");
        py::dict globals = mainMod.attr("__dict__");
        py::dict locals;

        if (!globals.contains("__builtins__")) {
            globals["__builtins__"] = py::module_::import("builtins");
        }

        // Inject math module
        locals["math"] = m_impl->mathModule;
        locals["time"] = context.timeSeconds;
        locals["frame"] = static_cast<int64_t>(context.frameIndex);

        // Inject 'me' node
        if (meNode) {
            locals["me"] = py::cast(meNode, py::return_value_policy::reference);
        }

        // Inject 'op' helper
        locals["op"] = py::cpp_function([this](const std::string& name) -> Node* {
            if (m_activeGraph) {
                return m_activeGraph->FindNode(name);
            }
            return nullptr;
        }, py::return_value_policy::reference);

        // Evaluate expression
        py::object result = py::eval(expression, globals, locals);

        // Convert result to PinValue
        if (py::isinstance<py::bool_>(result)) {
            return PinValue(result.cast<bool>());
        }
        if (py::isinstance<py::int_>(result)) {
            return PinValue(result.cast<int32_t>());
        }
        if (py::isinstance<py::float_>(result)) {
            return PinValue(result.cast<float>());
        }
        if (py::isinstance<py::str>(result)) {
            return PinValue(result.cast<std::string>());
        }
        if (py::isinstance<py::tuple>(result) || py::isinstance<py::list>(result)) {
            auto seq = result.cast<py::sequence>();
            if (seq.size() == 2) {
                return PinValue(glm::vec2(seq[0].cast<float>(), seq[1].cast<float>()));
            }
            if (seq.size() == 3) {
                return PinValue(glm::vec3(seq[0].cast<float>(), seq[1].cast<float>(), seq[2].cast<float>()));
            }
            if (seq.size() >= 4) {
                return PinValue(glm::vec4(seq[0].cast<float>(), seq[1].cast<float>(), seq[2].cast<float>(), seq[3].cast<float>()));
            }
        }

        return PinValue{};
    } catch (const py::error_already_set& e) {
        std::string err = e.what();
        spdlog::warn("Expression evaluation error in '{}': {}", expression, err);
        if (outError) *outError = err;
        return PinValue{};
    } catch (const std::exception& e) {
        std::string err = e.what();
        spdlog::warn("Exception in EvaluateExpression '{}': {}", expression, err);
        if (outError) *outError = err;
        return PinValue{};
    }
}

} // namespace nf
