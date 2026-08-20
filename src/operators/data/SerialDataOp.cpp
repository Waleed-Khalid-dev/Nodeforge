#include "SerialDataOp.h"
#include <chrono>
#include <iomanip>
#include <sstream>

namespace nf {

SerialDataOp::SerialDataOp(NodeId id, const std::string& name)
    : DataOp(id, name, "SerialDataOp"), m_serial(std::make_unique<SerialPort>()) {
    m_outPin = AddOutputPin("output", PinType::Data);

    SetParam("port", std::string("COM3"));
    SetParam("baud_rate", static_cast<int32_t>(115200));
    SetParam("delimiter", std::string("\n"));
    SetParam("max_rows", static_cast<int32_t>(100));
    SetParam("send_string", std::string(""));
    SetParam("active", true);

    m_cachedTable.SetColumnHeaders({ "timestamp", "message" });
}

SerialDataOp::~SerialDataOp() {
    if (m_serial) {
        m_serial->Close();
    }
}

void SerialDataOp::InjectMockData(const std::string& data) {
    if (m_serial) {
        m_serial->InjectMockData(data);
    }
}

bool SerialDataOp::SendString(const std::string& message) {
    if (m_serial && m_serial->IsOpen()) {
        return m_serial->Write(message);
    }
    return false;
}

bool SerialDataOp::Cook(const CookContext& /*context*/) {
    bool active = true;
    if (GetParam("active").Is<bool>()) {
        active = GetParam("active").Get<bool>();
    }

    std::string port = "COM3";
    if (GetParam("port").Is<std::string>()) {
        port = GetParam("port").Get<std::string>();
    }

    int32_t baud = 115200;
    if (GetParam("baud_rate").Is<int32_t>()) {
        baud = GetParam("baud_rate").Get<int32_t>();
    }

    std::string delimiter = "\n";
    if (GetParam("delimiter").Is<std::string>()) {
        delimiter = GetParam("delimiter").Get<std::string>();
    }

    int32_t maxRows = 100;
    if (GetParam("max_rows").Is<int32_t>()) {
        maxRows = std::max(1, GetParam("max_rows").Get<int32_t>());
    }

    if (active) {
        if (!m_serial->IsOpen() || m_lastPort != port || m_lastBaud != static_cast<uint32_t>(baud)) {
            SerialConfig cfg;
            cfg.portName = port;
            cfg.baudRate = static_cast<uint32_t>(baud);
            cfg.delimiter = delimiter;
            cfg.maxQueueLines = static_cast<size_t>(maxRows);
            m_serial->Open(cfg);
            m_lastPort = port;
            m_lastBaud = static_cast<uint32_t>(baud);
        }
    } else {
        if (m_serial->IsOpen()) {
            m_serial->Close();
        }
    }

    std::vector<std::string> newLines = m_serial->DrainLines();
    if (!newLines.empty()) {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream timeSS;
        timeSS << std::put_time(std::localtime(&in_time_t), "%H:%M:%S");
        std::string ts = timeSS.str();

        for (const auto& line : newLines) {
            m_cachedTable.AppendRow({ ts, line });
        }

        // Limit rows
        while (m_cachedTable.GetRowCount() > static_cast<size_t>(maxRows)) {
            m_cachedTable.DeleteRow(0);
        }
    }

    SetOutputTable(m_cachedTable);
    return true;
}

} // namespace nf
