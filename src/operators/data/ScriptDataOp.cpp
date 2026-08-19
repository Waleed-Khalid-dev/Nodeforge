#include "ScriptDataOp.h"
#include "../../python/PythonEngine.h"
#include <spdlog/spdlog.h>
#include <pybind11/pybind11.h>
#include <pybind11/eval.h>
#include <fstream>
#include <sstream>

namespace py = pybind11;

namespace nf {

ScriptDataOp::ScriptDataOp(NodeId id, const std::string& name)
    : DataOp(id, name, "ScriptDataOp") {
    m_inPin = AddInputPin("input", PinType::Data);
    m_outPin = AddOutputPin("output", PinType::Data);

    SetParam("script", std::string("def onCook(dat):\n    pass\n"));
    SetParam("file_path", std::string(""));
    SetParam("pulse_run", false);
}

bool ScriptDataOp::Cook(const CookContext& /*context*/) {
    const DataTable* inTable = GetInputTable(0);
    DataTable outTable = inTable ? *inTable : DataTable{};

    std::string filePath = GetParam("file_path").Is<std::string>() ? GetParam("file_path").Get<std::string>() : "";
    std::string scriptCode = GetParam("script").Is<std::string>() ? GetParam("script").Get<std::string>() : "";

    if (!filePath.empty()) {
        std::ifstream file(filePath);
        if (file.is_open()) {
            std::stringstream ss;
            ss << file.rdbuf();
            scriptCode = ss.str();
        }
    }

    if (scriptCode.empty()) {
        SetOutputTable(outTable);
        return true;
    }

    try {
        py::gil_scoped_acquire acquire;
        py::dict localScope;
        localScope["dat"] = py::cast(&outTable);

        py::exec(scriptCode, py::globals(), localScope);

        if (localScope.contains("onCook")) {
            py::object onCookFunc = localScope["onCook"];
            if (py::isinstance<py::function>(onCookFunc)) {
                onCookFunc(py::cast(&outTable));
            }
        }
    } catch (const py::error_already_set& e) {
        spdlog::error("[ScriptDataOp: {}] Python onCook error: {}", GetName(), e.what());
    } catch (const std::exception& e) {
        spdlog::error("[ScriptDataOp: {}] Execution error: {}", GetName(), e.what());
    }

    SetOutputTable(outTable);
    return true;
}

void ScriptDataOp::Pulse(const std::string& paramName) {
    DataTable outTable = m_cachedTable;
    std::string scriptCode = GetParam("script").Is<std::string>() ? GetParam("script").Get<std::string>() : "";

    if (scriptCode.empty()) return;

    try {
        py::gil_scoped_acquire acquire;
        py::dict localScope;
        localScope["dat"] = py::cast(&outTable);

        py::exec(scriptCode, py::globals(), localScope);

        if (localScope.contains("onPulse")) {
            py::object onPulseFunc = localScope["onPulse"];
            if (py::isinstance<py::function>(onPulseFunc)) {
                onPulseFunc(py::cast(&outTable), paramName);
            }
        }
    } catch (const py::error_already_set& e) {
        spdlog::error("[ScriptDataOp: {}] Python onPulse error: {}", GetName(), e.what());
    } catch (const std::exception& e) {
        spdlog::error("[ScriptDataOp: {}] Execution error: {}", GetName(), e.what());
    }

    SetOutputTable(outTable);
}

} // namespace nf
