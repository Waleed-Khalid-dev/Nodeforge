#include "DataTable.h"
#include <sstream>
#include <algorithm>
#include <nlohmann/json.hpp>

namespace nf {

DataTable::DataTable(size_t rows, size_t cols, const std::string& defaultValue) {
    Resize(rows, cols, defaultValue);
}

DataTable::DataTable(const std::vector<std::string>& colHeaders, size_t rows)
    : m_colHeaders(colHeaders), m_colCount(colHeaders.size()) {
    RebuildHeaderMaps();
    m_rows.resize(rows, std::vector<std::string>(m_colCount, ""));
}

void DataTable::Resize(size_t rows, size_t cols, const std::string& defaultValue) {
    m_colCount = cols;
    m_rows.resize(rows);
    for (auto& row : m_rows) {
        row.resize(cols, defaultValue);
    }
    if (m_colHeaders.size() < cols) {
        for (size_t i = m_colHeaders.size(); i < cols; ++i) {
            m_colHeaders.push_back("col" + std::to_string(i));
        }
    } else if (m_colHeaders.size() > cols) {
        m_colHeaders.resize(cols);
    }
    RebuildHeaderMaps();
}

void DataTable::Clear() {
    m_rows.clear();
    m_colHeaders.clear();
    m_rowHeaders.clear();
    m_colMap.clear();
    m_rowMap.clear();
    m_colCount = 0;
}

void DataTable::SetColumnHeaders(const std::vector<std::string>& headers) {
    m_colHeaders = headers;
    m_colCount = headers.size();
    for (auto& row : m_rows) {
        row.resize(m_colCount, "");
    }
    RebuildHeaderMaps();
}

void DataTable::SetRowHeaders(const std::vector<std::string>& headers) {
    m_rowHeaders = headers;
    RebuildHeaderMaps();
}

void DataTable::RebuildHeaderMaps() {
    m_colMap.clear();
    for (size_t i = 0; i < m_colHeaders.size(); ++i) {
        m_colMap[m_colHeaders[i]] = i;
    }
    m_rowMap.clear();
    for (size_t i = 0; i < m_rowHeaders.size(); ++i) {
        m_rowMap[m_rowHeaders[i]] = i;
    }
}

int DataTable::FindColIndex(const std::string& name) const {
    auto it = m_colMap.find(name);
    return it != m_colMap.end() ? static_cast<int>(it->second) : -1;
}

int DataTable::FindRowIndex(const std::string& name) const {
    auto it = m_rowMap.find(name);
    return it != m_rowMap.end() ? static_cast<int>(it->second) : -1;
}

std::string DataTable::GetCell(size_t row, size_t col) const {
    if (row < m_rows.size() && col < m_colCount && col < m_rows[row].size()) {
        return m_rows[row][col];
    }
    return "";
}

void DataTable::SetCell(size_t row, size_t col, const std::string& value) {
    if (row >= m_rows.size() || col >= m_colCount) {
        size_t newRows = std::max(m_rows.size(), row + 1);
        size_t newCols = std::max(m_colCount, col + 1);
        Resize(newRows, newCols);
    }
    m_rows[row][col] = value;
}

std::string DataTable::GetCell(const std::string& rowName, const std::string& colName) const {
    int r = FindRowIndex(rowName);
    int c = FindColIndex(colName);
    if (r >= 0 && c >= 0) {
        return GetCell(static_cast<size_t>(r), static_cast<size_t>(c));
    }
    return "";
}

void DataTable::SetCell(const std::string& rowName, const std::string& colName, const std::string& value) {
    int r = FindRowIndex(rowName);
    int c = FindColIndex(colName);
    if (r >= 0 && c >= 0) {
        SetCell(static_cast<size_t>(r), static_cast<size_t>(c), value);
    }
}

float DataTable::GetCellFloat(size_t row, size_t col, float defaultValue) const {
    std::string val = GetCell(row, col);
    if (val.empty()) return defaultValue;
    try {
        return std::stof(val);
    } catch (...) {
        return defaultValue;
    }
}

int32_t DataTable::GetCellInt(size_t row, size_t col, int32_t defaultValue) const {
    std::string val = GetCell(row, col);
    if (val.empty()) return defaultValue;
    try {
        return std::stoi(val);
    } catch (...) {
        return defaultValue;
    }
}

bool DataTable::GetCellBool(size_t row, size_t col, bool defaultValue) const {
    std::string val = GetCell(row, col);
    if (val == "1" || val == "true" || val == "True" || val == "TRUE") return true;
    if (val == "0" || val == "false" || val == "False" || val == "FALSE") return false;
    return defaultValue;
}

const std::vector<std::string>& DataTable::GetRow(size_t row) const {
    static const std::vector<std::string> emptyRow;
    if (row < m_rows.size()) {
        return m_rows[row];
    }
    return emptyRow;
}

std::vector<std::string> DataTable::GetColumn(size_t col) const {
    std::vector<std::string> column;
    if (col < m_colCount) {
        column.reserve(m_rows.size());
        for (const auto& r : m_rows) {
            column.push_back(col < r.size() ? r[col] : "");
        }
    }
    return column;
}

void DataTable::AppendRow(const std::vector<std::string>& row) {
    if (m_colCount == 0 && !row.empty()) {
        m_colCount = row.size();
        for (size_t i = 0; i < m_colCount; ++i) {
            m_colHeaders.push_back("col" + std::to_string(i));
        }
        RebuildHeaderMaps();
    }
    std::vector<std::string> newRow = row;
    newRow.resize(m_colCount, "");
    m_rows.push_back(std::move(newRow));
}

void DataTable::AppendColumn(const std::string& header, const std::vector<std::string>& columnData) {
    m_colHeaders.push_back(header);
    m_colCount = m_colHeaders.size();
    size_t requiredRows = std::max(m_rows.size(), columnData.size());
    m_rows.resize(requiredRows, std::vector<std::string>(m_colCount - 1, ""));

    for (size_t i = 0; i < requiredRows; ++i) {
        m_rows[i].push_back(i < columnData.size() ? columnData[i] : "");
    }
    RebuildHeaderMaps();
}

void DataTable::InsertRow(size_t index, const std::vector<std::string>& row) {
    if (index >= m_rows.size()) {
        AppendRow(row);
        return;
    }
    std::vector<std::string> newRow = row;
    newRow.resize(m_colCount, "");
    m_rows.insert(m_rows.begin() + index, std::move(newRow));
}

void DataTable::DeleteRow(size_t index) {
    if (index < m_rows.size()) {
        m_rows.erase(m_rows.begin() + index);
    }
}

void DataTable::DeleteColumn(size_t index) {
    if (index < m_colCount) {
        for (auto& row : m_rows) {
            if (index < row.size()) {
                row.erase(row.begin() + index);
            }
        }
        if (index < m_colHeaders.size()) {
            m_colHeaders.erase(m_colHeaders.begin() + index);
        }
        m_colCount--;
        RebuildHeaderMaps();
    }
}

DataTable DataTable::FromCSV(const std::string& text, char delimiter, bool hasHeaders) {
    DataTable table;
    std::vector<std::vector<std::string>> parsedRows;
    std::vector<std::string> currentRow;
    std::string currentCell;
    bool inQuotes = false;

    for (size_t i = 0; i < text.size(); ++i) {
        char ch = text[i];
        if (inQuotes) {
            if (ch == '"') {
                if (i + 1 < text.size() && text[i + 1] == '"') {
                    currentCell += '"';
                    i++; // skip escaped quote
                } else {
                    inQuotes = false;
                }
            } else {
                currentCell += ch;
            }
        } else {
            if (ch == '"') {
                inQuotes = true;
            } else if (ch == delimiter) {
                currentRow.push_back(currentCell);
                currentCell.clear();
            } else if (ch == '\r') {
                // Ignore carriage return
            } else if (ch == '\n') {
                currentRow.push_back(currentCell);
                currentCell.clear();
                parsedRows.push_back(std::move(currentRow));
                currentRow.clear();
            } else {
                currentCell += ch;
            }
        }
    }
    if (!currentCell.empty() || !currentRow.empty()) {
        currentRow.push_back(currentCell);
        parsedRows.push_back(std::move(currentRow));
    }

    if (parsedRows.empty()) {
        return table;
    }

    size_t startRow = 0;
    if (hasHeaders && !parsedRows.empty()) {
        table.SetColumnHeaders(parsedRows[0]);
        startRow = 1;
    } else {
        size_t maxCols = 0;
        for (const auto& r : parsedRows) maxCols = std::max(maxCols, r.size());
        std::vector<std::string> defaultHeaders;
        for (size_t i = 0; i < maxCols; ++i) defaultHeaders.push_back("col" + std::to_string(i));
        table.SetColumnHeaders(defaultHeaders);
    }

    for (size_t r = startRow; r < parsedRows.size(); ++r) {
        table.AppendRow(parsedRows[r]);
    }

    return table;
}

std::string DataTable::ToCSV(char delimiter, bool includeHeaders) const {
    std::ostringstream ss;
    auto writeCell = [&](const std::string& val, bool isLast) {
        bool needQuotes = val.find(delimiter) != std::string::npos ||
                          val.find('"') != std::string::npos ||
                          val.find('\n') != std::string::npos;
        if (needQuotes) {
            ss << '"';
            for (char c : val) {
                if (c == '"') ss << "\"\"";
                else ss << c;
            }
            ss << '"';
        } else {
            ss << val;
        }
        if (!isLast) ss << delimiter;
    };

    if (includeHeaders && !m_colHeaders.empty()) {
        for (size_t i = 0; i < m_colHeaders.size(); ++i) {
            writeCell(m_colHeaders[i], i + 1 == m_colHeaders.size());
        }
        ss << "\n";
    }

    for (const auto& row : m_rows) {
        for (size_t i = 0; i < m_colCount; ++i) {
            std::string val = (i < row.size()) ? row[i] : "";
            writeCell(val, i + 1 == m_colCount);
        }
        ss << "\n";
    }

    return ss.str();
}

DataTable DataTable::FromText(const std::string& text) {
    DataTable table;
    table.SetColumnHeaders({ "text" });
    std::stringstream ss(text);
    std::string line;
    while (std::getline(ss, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        table.AppendRow({ line });
    }
    return table;
}

std::string DataTable::ToText() const {
    std::ostringstream ss;
    for (size_t r = 0; r < m_rows.size(); ++r) {
        if (!m_rows[r].empty()) {
            ss << m_rows[r][0];
        }
        if (r + 1 < m_rows.size()) {
            ss << "\n";
        }
    }
    return ss.str();
}

DataTable DataTable::FromJSON(const std::string& jsonStr) {
    DataTable table;
    try {
        auto j = nlohmann::json::parse(jsonStr);
        if (j.is_array()) {
            if (!j.empty() && j[0].is_object()) {
                // Array of objects -> columns are object keys
                std::vector<std::string> keys;
                for (auto it = j[0].begin(); it != j[0].end(); ++it) {
                    keys.push_back(it.key());
                }
                table.SetColumnHeaders(keys);
                for (const auto& elem : j) {
                    std::vector<std::string> row;
                    for (const auto& k : keys) {
                        row.push_back(elem.contains(k) ? (elem[k].is_string() ? elem[k].get<std::string>() : elem[k].dump()) : "");
                    }
                    table.AppendRow(row);
                }
            } else {
                table.SetColumnHeaders({ "value" });
                for (const auto& elem : j) {
                    table.AppendRow({ elem.is_string() ? elem.get<std::string>() : elem.dump() });
                }
            }
        } else if (j.is_object()) {
            table.SetColumnHeaders({ "key", "value" });
            for (auto it = j.begin(); it != j.end(); ++it) {
                table.AppendRow({ it.key(), it.value().is_string() ? it.value().get<std::string>() : it.value().dump() });
            }
        }
    } catch (...) {
        return FromText(jsonStr);
    }
    return table;
}

std::string DataTable::ToJSON(bool pretty) const {
    if (m_colHeaders.size() == 2 && m_colHeaders[0] == "key" && m_colHeaders[1] == "value") {
        nlohmann::json j = nlohmann::json::object();
        for (const auto& row : m_rows) {
            if (row.size() >= 2) {
                j[row[0]] = row[1];
            }
        }
        return pretty ? j.dump(2) : j.dump();
    } else {
        nlohmann::json jArr = nlohmann::json::array();
        for (const auto& row : m_rows) {
            nlohmann::json obj = nlohmann::json::object();
            for (size_t c = 0; c < m_colHeaders.size(); ++c) {
                obj[m_colHeaders[c]] = (c < row.size()) ? row[c] : "";
            }
            jArr.push_back(obj);
        }
        return pretty ? jArr.dump(2) : jArr.dump();
    }
}

} // namespace nf
