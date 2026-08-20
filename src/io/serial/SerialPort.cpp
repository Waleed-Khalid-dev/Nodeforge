#include "SerialPort.h"
#include <spdlog/spdlog.h>
#include <chrono>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace nf {

struct SerialPort::PlatformHandle {
    HANDLE hPort = INVALID_HANDLE_VALUE;
    OVERLAPPED ovRead{};
    OVERLAPPED ovWrite{};

    PlatformHandle() {
        ovRead.hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
        ovWrite.hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    }

    ~PlatformHandle() {
        if (hPort != INVALID_HANDLE_VALUE) {
            CloseHandle(hPort);
            hPort = INVALID_HANDLE_VALUE;
        }
        if (ovRead.hEvent) CloseEvent(ovRead.hEvent);
        if (ovWrite.hEvent) CloseEvent(ovWrite.hEvent);
    }
private:
    void CloseEvent(HANDLE& h) {
        if (h) {
            CloseHandle(h);
            h = nullptr;
        }
    }
};

} // namespace nf
#else
namespace nf {
struct SerialPort::PlatformHandle {
    int fd = -1;
};
}
#endif

namespace nf {

SerialPort::SerialPort()
    : m_handle(std::make_unique<PlatformHandle>()) {
}

SerialPort::SerialPort(const SerialConfig& config)
    : m_config(config), m_handle(std::make_unique<PlatformHandle>()) {
    Open(config);
}

SerialPort::~SerialPort() {
    Close();
}

bool SerialPort::Open(const SerialConfig& config) {
    Close();
    m_config = config;

#ifdef _WIN32
    std::string deviceName = "\\\\.\\" + config.portName;
    m_handle->hPort = CreateFileA(
        deviceName.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0,
        nullptr,
        OPEN_EXISTING,
        FILE_FLAG_OVERLAPPED,
        nullptr
    );

    if (m_handle->hPort == INVALID_HANDLE_VALUE) {
        spdlog::debug("SerialPort: Could not open port {} (will run in simulation/standby)", config.portName);
        m_isOpen = false;
        return false;
    }

    DCB dcb{};
    dcb.DCBlength = sizeof(DCB);
    if (!GetCommState(m_handle->hPort, &dcb)) {
        Close();
        return false;
    }

    dcb.BaudRate = config.baudRate;
    dcb.ByteSize = config.dataBits;
    dcb.StopBits = (config.stopBits == 2) ? TWOSTOPBITS : ONESTOPBIT;
    dcb.Parity = (config.parity == 1) ? ODDPARITY : (config.parity == 2 ? EVENPARITY : NOPARITY);
    dcb.fBinary = TRUE;
    dcb.fDtrControl = DTR_CONTROL_ENABLE;
    dcb.fRtsControl = RTS_CONTROL_ENABLE;

    if (!SetCommState(m_handle->hPort, &dcb)) {
        Close();
        return false;
    }

    COMMTIMEOUTS timeouts{};
    timeouts.ReadIntervalTimeout = MAXDWORD;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.ReadTotalTimeoutConstant = 100;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 500;
    SetCommTimeouts(m_handle->hPort, &timeouts);

    PurgeComm(m_handle->hPort, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
    m_isOpen = true;
#else
    m_isOpen = false;
#endif

    m_running = true;
    m_readerThread = std::thread(&SerialPort::ReaderThreadFunc, this);
    spdlog::info("SerialPort: Connected to {} @ {} baud", config.portName, config.baudRate);
    return true;
}

void SerialPort::Close() {
    m_running = false;
    if (m_readerThread.joinable()) {
        m_readerThread.join();
    }
#ifdef _WIN32
    if (m_handle && m_handle->hPort != INVALID_HANDLE_VALUE) {
        CloseHandle(m_handle->hPort);
        m_handle->hPort = INVALID_HANDLE_VALUE;
    }
#endif
    m_isOpen = false;
}

bool SerialPort::IsOpen() const {
    return m_isOpen;
}

bool SerialPort::Write(const std::string& message) {
    return WriteBytes(reinterpret_cast<const uint8_t*>(message.data()), message.size());
}

bool SerialPort::WriteBytes(const uint8_t* data, size_t size) {
    if (!m_isOpen || !data || size == 0) return false;

#ifdef _WIN32
    DWORD bytesWritten = 0;
    ResetEvent(m_handle->ovWrite.hEvent);
    if (!WriteFile(m_handle->hPort, data, static_cast<DWORD>(size), &bytesWritten, &m_handle->ovWrite)) {
        if (GetLastError() == ERROR_IO_PENDING) {
            GetOverlappedResult(m_handle->hPort, &m_handle->ovWrite, &bytesWritten, TRUE);
        } else {
            return false;
        }
    }
    return bytesWritten == size;
#else
    (void)data; (void)size;
    return false;
#endif
}

std::vector<std::string> SerialPort::DrainLines() {
    std::lock_guard<std::mutex> lock(m_queueMutex);
    std::vector<std::string> lines(m_lineQueue.begin(), m_lineQueue.end());
    m_lineQueue.clear();
    return lines;
}

std::string SerialPort::GetLatestLine() const {
    std::lock_guard<std::mutex> lock(m_queueMutex);
    return m_latestLine;
}

std::vector<uint8_t> SerialPort::DrainRawBytes() {
    std::lock_guard<std::mutex> lock(m_queueMutex);
    std::vector<uint8_t> bytes(m_rawByteQueue.begin(), m_rawByteQueue.end());
    m_rawByteQueue.clear();
    return bytes;
}

void SerialPort::InjectMockData(const std::string& mockData) {
    std::lock_guard<std::mutex> lock(m_queueMutex);
    for (char c : mockData) {
        m_rawByteQueue.push_back(static_cast<uint8_t>(c));
    }
    m_incomingBuffer += mockData;

    std::string delim = m_config.delimiter.empty() ? "\n" : m_config.delimiter;
    size_t pos = 0;
    while ((pos = m_incomingBuffer.find(delim)) != std::string::npos) {
        std::string line = m_incomingBuffer.substr(0, pos);
        // Strip trailing \r
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        m_latestLine = line;
        if (m_lineQueue.size() >= m_config.maxQueueLines) {
            m_lineQueue.pop_front();
        }
        m_lineQueue.push_back(line);
        m_incomingBuffer.erase(0, pos + delim.length());
    }
}

void SerialPort::ReaderThreadFunc() {
#ifdef _WIN32
    uint8_t buffer[512];
    while (m_running && m_isOpen) {
        DWORD bytesRead = 0;
        ResetEvent(m_handle->ovRead.hEvent);

        if (!ReadFile(m_handle->hPort, buffer, sizeof(buffer), &bytesRead, &m_handle->ovRead)) {
            if (GetLastError() == ERROR_IO_PENDING) {
                if (WaitForSingleObject(m_handle->ovRead.hEvent, 50) == WAIT_OBJECT_0) {
                    GetOverlappedResult(m_handle->hPort, &m_handle->ovRead, &bytesRead, FALSE);
                }
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                continue;
            }
        }

        if (bytesRead > 0) {
            std::string chunk(reinterpret_cast<char*>(buffer), bytesRead);
            InjectMockData(chunk);
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }
#else
    while (m_running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
#endif
}

} // namespace nf
