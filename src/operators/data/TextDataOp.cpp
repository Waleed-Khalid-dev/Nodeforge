#include "TextDataOp.h"
#include <fstream>
#include <sstream>

namespace nf {

TextDataOp::TextDataOp(NodeId id, const std::string& name)
    : DataOp(id, name, "TextDataOp") {
    m_outPin = AddOutputPin("output", PinType::Data);

    SetParam("text", std::string(""));
    SetParam("file_path", std::string(""));
    SetParam("auto_reload", false);
}

bool TextDataOp::Cook(const CookContext& /*context*/) {
    std::string filePath = GetParam("file_path").Is<std::string>() ? GetParam("file_path").Get<std::string>() : "";
    std::string textContent = GetParam("text").Is<std::string>() ? GetParam("text").Get<std::string>() : "";

    if (!filePath.empty()) {
        std::ifstream file(filePath);
        if (file.is_open()) {
            std::stringstream ss;
            ss << file.rdbuf();
            textContent = ss.str();
        }
    }

    DataTable table = DataTable::FromText(textContent);
    SetOutputTable(table);
    return true;
}

} // namespace nf
