#pragma once

#include "DataOp.h"
#include "io/serial/SerialPort.h"
#include <memory>

namespace nf {

class SerialDataOp : public DataOp {
public:
    SerialDataOp(NodeId id, const std::string& name);
    ~SerialDataOp() override;

    bool Cook(const CookContext& context) override;

    void InjectMockData(const std::string& data);
    bool SendString(const std::string& message);

private:
    std::unique_ptr<SerialPort> m_serial;
    std::string m_lastPort;
    uint32_t m_lastBaud = 0;
};

} // namespace nf
