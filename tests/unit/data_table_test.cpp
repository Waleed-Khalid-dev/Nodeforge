#include <gtest/gtest.h>
#include "core/DataTable.h"

using namespace nf;

TEST(DataTableTest, ConstructionAndDimensions) {
    DataTable table(4, 3, "init");
    EXPECT_EQ(table.GetRowCount(), 4);
    EXPECT_EQ(table.GetColumnCount(), 3);
    EXPECT_FALSE(table.IsEmpty());

    EXPECT_EQ(table.GetCell(0, 0), "init");
    EXPECT_EQ(table.GetCell(3, 2), "init");
}

TEST(DataTableTest, HeadersAndNamedLookup) {
    std::vector<std::string> headers = { "name", "x", "y", "z" };
    DataTable table(headers, 0);

    EXPECT_EQ(table.GetColumnCount(), 4);
    EXPECT_EQ(table.FindColIndex("name"), 0);
    EXPECT_EQ(table.FindColIndex("y"), 2);
    EXPECT_EQ(table.FindColIndex("missing"), -1);

    table.AppendRow({ "target1", "10.5", "20.25", "30.0" });
    table.SetRowHeaders({ "rowA" });

    EXPECT_EQ(table.FindRowIndex("rowA"), 0);
    EXPECT_EQ(table.GetCell("rowA", "name"), "target1");
    EXPECT_FLOAT_EQ(table.GetCellFloat(0, 1), 10.5f);
    EXPECT_FLOAT_EQ(table.GetCellFloat(0, 2), 20.25f);
    EXPECT_FLOAT_EQ(table.GetCellFloat(0, 3), 30.0f);
}

TEST(DataTableTest, CSVImportAndExport) {
    std::string csv = "id,name,value\n1,\"Projector, Main\",100.5\n2,\"Sensor \\\"A\\\"\",200.0\n";
    DataTable table = DataTable::FromCSV(csv, ',', true);

    EXPECT_EQ(table.GetRowCount(), 2);
    EXPECT_EQ(table.GetColumnCount(), 3);
    EXPECT_EQ(table.GetCell(0, 1), "Projector, Main");
    EXPECT_FLOAT_EQ(table.GetCellFloat(0, 2), 100.5f);

    std::string exported = table.ToCSV(',', true);
    DataTable table2 = DataTable::FromCSV(exported, ',', true);

    EXPECT_EQ(table2.GetRowCount(), 2);
    EXPECT_EQ(table2.GetCell(0, 1), "Projector, Main");
}

TEST(DataTableTest, RawTextAndJSON) {
    std::string text = "line 1\nline 2\nline 3";
    DataTable tTable = DataTable::FromText(text);
    EXPECT_EQ(tTable.GetRowCount(), 3);
    EXPECT_EQ(tTable.GetCell(1, 0), "line 2");
    EXPECT_EQ(tTable.ToText(), text);

    std::string json = "{\"name\":\"NodeForge\",\"active\":\"true\"}";
    DataTable jTable = DataTable::FromJSON(json);
    EXPECT_EQ(jTable.GetRowCount(), 2);
    EXPECT_EQ(jTable.GetCell("0", "key"), ""); // Key-value format
}
