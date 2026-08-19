#include "SelectDataOp.h"
#include <sstream>
#include <algorithm>

namespace nf {

SelectDataOp::SelectDataOp(NodeId id, const std::string& name)
    : DataOp(id, name, "SelectDataOp") {
    m_inPin = AddInputPin("input", PinType::Data);
    m_outPin = AddOutputPin("output", PinType::Data);

    SetParam("col_names", std::string("*"));
    SetParam("start_row", static_cast<int32_t>(0));
    SetParam("num_rows", static_cast<int32_t>(-1)); // -1 = all
    SetParam("include_headers", true);
}

bool SelectDataOp::Cook(const CookContext& /*context*/) {
    const DataTable* inTable = GetInputTable(0);
    if (!inTable || inTable->IsEmpty()) {
        SetOutputTable(DataTable{});
        return true;
    }

    std::string colNamesStr = GetParam("col_names").Is<std::string>() ? GetParam("col_names").Get<std::string>() : "*";
    int32_t startRowParam = GetParam("start_row").Is<int32_t>() ? GetParam("start_row").Get<int32_t>() : 0;
    int32_t numRowsParam = GetParam("num_rows").Is<int32_t>() ? GetParam("num_rows").Get<int32_t>() : -1;

    size_t startRow = static_cast<size_t>(std::max(0, startRowParam));
    size_t totalInRows = inTable->GetRowCount();

    size_t endRow = (numRowsParam < 0) ? totalInRows : std::min(totalInRows, startRow + static_cast<size_t>(numRowsParam));

    // Determine matching columns
    std::vector<size_t> matchedColIndices;
    std::vector<std::string> outColHeaders;

    if (colNamesStr == "*" || colNamesStr.empty()) {
        for (size_t c = 0; c < inTable->GetColumnCount(); ++c) {
            matchedColIndices.push_back(c);
            if (c < inTable->GetColumnHeaders().size()) {
                outColHeaders.push_back(inTable->GetColumnHeaders()[c]);
            } else {
                outColHeaders.push_back("col" + std::to_string(c));
            }
        }
    } else {
        std::stringstream ss(colNamesStr);
        std::string pattern;
        while (ss >> pattern) {
            int idx = inTable->FindColIndex(pattern);
            if (idx >= 0) {
                matchedColIndices.push_back(static_cast<size_t>(idx));
                outColHeaders.push_back(pattern);
            } else {
                // Try integer column index
                try {
                    int cIdx = std::stoi(pattern);
                    if (cIdx >= 0 && static_cast<size_t>(cIdx) < inTable->GetColumnCount()) {
                        matchedColIndices.push_back(static_cast<size_t>(cIdx));
                        outColHeaders.push_back(inTable->GetColumnHeaders()[cIdx]);
                    }
                } catch (...) {}
            }
        }
    }

    if (matchedColIndices.empty()) {
        SetOutputTable(DataTable{});
        return true;
    }

    DataTable outTable(outColHeaders, 0);

    for (size_t r = startRow; r < endRow; ++r) {
        std::vector<std::string> rowData;
        for (size_t cIdx : matchedColIndices) {
            rowData.push_back(inTable->GetCell(r, cIdx));
        }
        outTable.AppendRow(rowData);
    }

    SetOutputTable(outTable);
    return true;
}

} // namespace nf
