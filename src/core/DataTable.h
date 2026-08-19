#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <span>

namespace nf {

class DataTable {
public:
    DataTable() = default;
    DataTable(size_t rows, size_t cols, const std::string& defaultValue = "");
    DataTable(const std::vector<std::string>& colHeaders, size_t rows = 0);

    // Dimensions
    size_t GetRowCount() const { return m_rows.size(); }
    size_t GetColumnCount() const { return m_colCount; }
    bool IsEmpty() const { return m_rows.empty() || m_colCount == 0; }
    void Resize(size_t rows, size_t cols, const std::string& defaultValue = "");
    void Clear();

    // Headers
    const std::vector<std::string>& GetColumnHeaders() const { return m_colHeaders; }
    void SetColumnHeaders(const std::vector<std::string>& headers);
    const std::vector<std::string>& GetRowHeaders() const { return m_rowHeaders; }
    void SetRowHeaders(const std::vector<std::string>& headers);

    int FindColIndex(const std::string& name) const;
    int FindRowIndex(const std::string& name) const;

    // Cell Accessors (by index)
    std::string GetCell(size_t row, size_t col) const;
    void SetCell(size_t row, size_t col, const std::string& value);

    // Cell Accessors (by header name)
    std::string GetCell(const std::string& rowName, const std::string& colName) const;
    void SetCell(const std::string& rowName, const std::string& colName, const std::string& value);

    // Typed cell conversions with fallback
    float GetCellFloat(size_t row, size_t col, float defaultValue = 0.0f) const;
    int32_t GetCellInt(size_t row, size_t col, int32_t defaultValue = 0) const;
    bool GetCellBool(size_t row, size_t col, bool defaultValue = false) const;

    // Row & Column Operations
    const std::vector<std::string>& GetRow(size_t row) const;
    std::vector<std::string> GetColumn(size_t col) const;
    void AppendRow(const std::vector<std::string>& row);
    void AppendColumn(const std::string& header, const std::vector<std::string>& columnData = {});
    void InsertRow(size_t index, const std::vector<std::string>& row);
    void DeleteRow(size_t index);
    void DeleteColumn(size_t index);

    // CSV / TSV Parsing & Formatting (RFC 4180 compliant)
    static DataTable FromCSV(const std::string& text, char delimiter = ',', bool hasHeaders = true);
    std::string ToCSV(char delimiter = ',', bool includeHeaders = true) const;

    // Raw Multiline Text
    static DataTable FromText(const std::string& text);
    std::string ToText() const;

    // JSON String
    static DataTable FromJSON(const std::string& jsonStr);
    std::string ToJSON(bool pretty = true) const;

    // Equality comparison
    bool operator==(const DataTable& other) const = default;

private:
    std::vector<std::vector<std::string>> m_rows;
    std::vector<std::string> m_colHeaders;
    std::vector<std::string> m_rowHeaders;
    size_t m_colCount = 0;

    void RebuildHeaderMaps();
    std::unordered_map<std::string, size_t> m_colMap;
    std::unordered_map<std::string, size_t> m_rowMap;
};

} // namespace nf
