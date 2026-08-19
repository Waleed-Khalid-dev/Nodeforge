#include "DataOp.h"

namespace nf {

DataOp::DataOp(NodeId id, const std::string& name, const std::string& typeName)
    : Node(id, name, typeName) {
}

const DataTable* DataOp::GetOutputTable() const {
    if (m_outPin && m_outPin->GetValue().Is<DataTable>()) {
        return &m_outPin->GetValue().Get<DataTable>();
    }
    return nullptr;
}

const DataTable* DataOp::GetInputTable(size_t index) const {
    if (index < m_inputPins.size()) {
        const auto& val = m_inputPins[index]->GetValue();
        if (val.Is<DataTable>()) {
            return &val.Get<DataTable>();
        }
    }
    return nullptr;
}

std::string DataOp::GetCell(size_t row, size_t col) const {
    const DataTable* table = GetOutputTable();
    return table ? table->GetCell(row, col) : "";
}

std::string DataOp::GetCell(const std::string& rowName, const std::string& colName) const {
    const DataTable* table = GetOutputTable();
    return table ? table->GetCell(rowName, colName) : "";
}

size_t DataOp::GetRowCount() const {
    const DataTable* table = GetOutputTable();
    return table ? table->GetRowCount() : 0;
}

size_t DataOp::GetColumnCount() const {
    const DataTable* table = GetOutputTable();
    return table ? table->GetColumnCount() : 0;
}

void DataOp::SetOutputTable(const DataTable& table) {
    m_cachedTable = table;
    if (m_outPin) {
        m_outPin->SetValue(PinValue(m_cachedTable));
    }
}

} // namespace nf
