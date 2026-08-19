#include "MergeDataOp.h"
#include <algorithm>

namespace nf {

MergeDataOp::MergeDataOp(NodeId id, const std::string& name)
    : DataOp(id, name, "MergeDataOp") {
    m_inPin1 = AddInputPin("input1", PinType::Data);
    m_inPin2 = AddInputPin("input2", PinType::Data);
    m_outPin = AddOutputPin("output", PinType::Data);

    SetParam("mode", static_cast<int32_t>(0)); // 0: Append Rows (Vertical), 1: Append Columns (Horizontal)
    SetParam("align_headers", true);
}

bool MergeDataOp::Cook(const CookContext& /*context*/) {
    const DataTable* table1 = GetInputTable(0);
    const DataTable* table2 = GetInputTable(1);

    if (!table1 && !table2) {
        SetOutputTable(DataTable{});
        return true;
    }
    if (!table1) {
        SetOutputTable(*table2);
        return true;
    }
    if (!table2) {
        SetOutputTable(*table1);
        return true;
    }

    int32_t mode = GetParam("mode").Is<int32_t>() ? GetParam("mode").Get<int32_t>() : 0;
    DataTable outTable;

    if (mode == 0) {
        // Vertical: Append Rows
        outTable = *table1;
        bool alignHeaders = GetParam("align_headers").Is<bool>() ? GetParam("align_headers").Get<bool>() : true;

        if (alignHeaders && !table2->GetColumnHeaders().empty()) {
            for (size_t r = 0; r < table2->GetRowCount(); ++r) {
                std::vector<std::string> rowData;
                for (const auto& header : outTable.GetColumnHeaders()) {
                    int cIdx = table2->FindColIndex(header);
                    rowData.push_back(cIdx >= 0 ? table2->GetCell(r, static_cast<size_t>(cIdx)) : "");
                }
                outTable.AppendRow(rowData);
            }
        } else {
            for (size_t r = 0; r < table2->GetRowCount(); ++r) {
                outTable.AppendRow(table2->GetRow(r));
            }
        }
    } else {
        // Horizontal: Append Columns
        std::vector<std::string> combinedHeaders = table1->GetColumnHeaders();
        for (const auto& h : table2->GetColumnHeaders()) {
            combinedHeaders.push_back(h);
        }
        outTable.SetColumnHeaders(combinedHeaders);

        size_t totalRows = std::max(table1->GetRowCount(), table2->GetRowCount());
        for (size_t r = 0; r < totalRows; ++r) {
            std::vector<std::string> rowData;
            for (size_t c = 0; c < table1->GetColumnCount(); ++c) {
                rowData.push_back(table1->GetCell(r, c));
            }
            for (size_t c = 0; c < table2->GetColumnCount(); ++c) {
                rowData.push_back(table2->GetCell(r, c));
            }
            outTable.AppendRow(rowData);
        }
    }

    SetOutputTable(outTable);
    return true;
}

} // namespace nf
