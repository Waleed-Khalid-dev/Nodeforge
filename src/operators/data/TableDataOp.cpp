#include "TableDataOp.h"
#include <fstream>
#include <sstream>

namespace nf {

TableDataOp::TableDataOp(NodeId id, const std::string& name)
    : DataOp(id, name, "TableDataOp") {
    m_outPin = AddOutputPin("output", PinType::Data);

    SetParam("file_path", std::string(""));
    SetParam("delimiter", std::string(","));
    SetParam("has_headers", true);
    SetParam("raw_csv", std::string("col0,col1,col2\n10,20,30\n40,50,60"));
}

bool TableDataOp::Cook(const CookContext& /*context*/) {
    std::string filePath = GetParam("file_path").Is<std::string>() ? GetParam("file_path").Get<std::string>() : "";
    std::string delimStr = GetParam("delimiter").Is<std::string>() ? GetParam("delimiter").Get<std::string>() : ",";
    char delimiter = delimStr.empty() ? ',' : delimStr[0];
    bool hasHeaders = GetParam("has_headers").Is<bool>() ? GetParam("has_headers").Get<bool>() : true;
    std::string csvContent = GetParam("raw_csv").Is<std::string>() ? GetParam("raw_csv").Get<std::string>() : "";

    if (!filePath.empty()) {
        std::ifstream file(filePath);
        if (file.is_open()) {
            std::stringstream ss;
            ss << file.rdbuf();
            csvContent = ss.str();
        }
    }

    DataTable table = DataTable::FromCSV(csvContent, delimiter, hasHeaders);
    SetOutputTable(table);
    return true;
}

} // namespace nf
