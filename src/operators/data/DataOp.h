#pragma once

#include "../../graph/Node.h"
#include "../../core/DataTable.h"

namespace nf {

class DataOp : public Node {
public:
    DataOp(NodeId id, const std::string& name, const std::string& typeName);
    virtual ~DataOp() = default;

    const DataTable* GetOutputTable() const;
    const DataTable* GetInputTable(size_t index = 0) const;

    std::string GetCell(size_t row, size_t col) const;
    std::string GetCell(const std::string& rowName, const std::string& colName) const;
    size_t GetRowCount() const;
    size_t GetColumnCount() const;

    void SetOutputTable(const DataTable& table);

protected:
    DataTable m_cachedTable;
    Pin* m_outPin = nullptr;
};

} // namespace nf
