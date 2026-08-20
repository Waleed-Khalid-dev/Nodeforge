#include <gtest/gtest.h>
#include "core/DataTable.h"
#include "io/serial/SerialPort.h"
#include "operators/data/SerialDataOp.h"

using namespace nf;

TEST(SerialProtocolTest, SerialPortMockLineInjection) {
    SerialConfig cfg;
    cfg.portName = "COM99";
    cfg.baudRate = 115200;
    cfg.delimiter = "\n";
    cfg.maxQueueLines = 10;

    SerialPort serial(cfg);
    serial.InjectMockData("SENSOR,1024,3.14\r\nSTATUS,OK\n");

    std::vector<std::string> lines = serial.DrainLines();
    ASSERT_EQ(lines.size(), 2u);
    EXPECT_EQ(lines[0], "SENSOR,1024,3.14");
    EXPECT_EQ(lines[1], "STATUS,OK");

    EXPECT_EQ(serial.GetLatestLine(), "STATUS,OK");
}

TEST(SerialProtocolTest, SerialDataOpCookAndTableLog) {
    SerialDataOp op(1, "serial1");
    op.SetParam("active", false); // Do not open real COM hardware in unit tests
    op.SetParam("max_rows", static_cast<int32_t>(5));

    op.InjectMockData("PRESSURE,101.3\n");
    op.InjectMockData("TEMP,24.5\n");
    op.InjectMockData("HUMIDITY,55.0\n");

    CookContext ctx;
    EXPECT_TRUE(op.Cook(ctx));

    Pin* outPin = op.GetOutputPin("output");
    ASSERT_NE(outPin, nullptr);
    DataTable table = outPin->GetValue().Get<DataTable>();

    EXPECT_EQ(table.GetRowCount(), 3u);
    EXPECT_EQ(table.GetColumnCount(), 2u); // timestamp, message
    EXPECT_EQ(table.GetCell(0, 1), "PRESSURE,101.3");
    EXPECT_EQ(table.GetCell(1, 1), "TEMP,24.5");
    EXPECT_EQ(table.GetCell(2, 1), "HUMIDITY,55.0");
}
