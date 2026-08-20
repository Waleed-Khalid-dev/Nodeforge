#pragma once

#include <string>
#include <vector>
#include <deque>
#include <mutex>
#include <atomic>
#include <thread>
#include <memory>
#include <cstdint>

namespace nf {

struct SerialConfig {
    std::string portName = "COM3";
    uint32_t baudRate = 115200;
    uint8_t dataBits = 8;
    uint8_t stopBits = 1; // 1 or 2
    uint8_t parity = 0;   // 0 = None, 1 = Odd, 2 = Even
    std::string delimiter = "\n";
    size_t maxQueueLines = 1000;
};

class SerialPort {
public:
    SerialPort();
    explicit SerialPort(const SerialConfig& config);
    ~SerialPort();

    bool Open(const SerialConfig& config);
    void Close();
    bool IsOpen() const;

    bool Write(const std::string& message);
    bool WriteBytes(const uint8_t* data, size_t size);

    std::vector<std::string> DrainLines();
    std::string GetLatestLine() const;
    std::vector<uint8_t> DrainRawBytes();

    // Testing simulation
    void InjectMockData(const std::string& mockData);

    const SerialConfig& GetConfig() const { return m_config; }

private:
    void ReaderThreadFunc();

    SerialConfig m_config;
    std::atomic<bool> m_running{false};
    std::atomic<bool> m_isOpen{false};
    std::thread m_readerThread;

    mutable std::mutex m_queueMutex;
    std::deque<std::string> m_lineQueue;
    std::deque<uint8_t> m_rawByteQueue;
    std::string m_latestLine;
    std::string m_incomingBuffer;

    struct PlatformHandle;
    std::unique_ptr<PlatformHandle> m_handle;
};

} // namespace nf
