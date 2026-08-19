#include "JSONDataOp.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>

namespace nf {

JSONDataOp::JSONDataOp(NodeId id, const std::string& name)
    : DataOp(id, name, "JSONDataOp") {
    m_inPin = AddInputPin("input", PinType::Data);
    m_outPin = AddOutputPin("output", PinType::Data);

    SetParam("json_text", std::string("{\n  \"name\": \"NodeForge\",\n  \"version\": \"0.1.0\",\n  \"active\": true\n}"));
    SetParam("file_path", std::string(""));
    SetParam("json_pointer", std::string(""));
    SetParam("mode", static_cast<int32_t>(1)); // 0: Raw Text, 1: Key-Value, 2: Array Table
}

bool JSONDataOp::Cook(const CookContext& /*context*/) {
    const DataTable* inTable = GetInputTable(0);
    std::string jsonStr = inTable ? inTable->ToText() : "";

    std::string filePath = GetParam("file_path").Is<std::string>() ? GetParam("file_path").Get<std::string>() : "";
    if (!filePath.empty()) {
        std::ifstream file(filePath);
        if (file.is_open()) {
            std::stringstream ss;
            ss << file.rdbuf();
            jsonStr = ss.str();
        }
    } else if (jsonStr.empty()) {
        jsonStr = GetParam("json_text").Is<std::string>() ? GetParam("json_text").Get<std::string>() : "{}";
    }

    std::string jsonPointer = GetParam("json_pointer").Is<std::string>() ? GetParam("json_pointer").Get<std::string>() : "";
    int32_t mode = GetParam("mode").Is<int32_t>() ? GetParam("mode").Get<int32_t>() : 1;

    DataTable outTable;
    try {
        auto j = nlohmann::json::parse(jsonStr);
        if (!jsonPointer.empty()) {
            try {
                j = j[nlohmann::json::json_pointer(jsonPointer)];
            } catch (...) {
                // Pointer not found
            }
        }

        if (mode == 0) {
            outTable = DataTable::FromText(j.dump(2));
        } else if (mode == 1 && j.is_object()) {
            outTable.SetColumnHeaders({ "key", "value" });
            for (auto it = j.begin(); it != j.end(); ++it) {
                outTable.AppendRow({ it.key(), it.value().is_string() ? it.value().get<std::string>() : it.value().dump() });
            }
        } else if (mode == 2 && j.is_array()) {
            outTable = DataTable::FromJSON(j.dump());
        } else {
            outTable = DataTable::FromJSON(j.dump());
        }
    } catch (...) {
        outTable = DataTable::FromText(jsonStr);
    }

    SetOutputTable(outTable);
    return true;
}

} // namespace nf
