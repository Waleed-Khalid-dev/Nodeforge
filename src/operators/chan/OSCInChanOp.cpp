#include "OSCInChanOp.h"
#include <spdlog/spdlog.h>
#include <algorithm>
#include <cstring>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#define SOCKET int
#define INVALID_SOCKET -1
#define closesocket close
#endif

namespace nf {

static uint32_t Swap32(uint32_t val) {
    return ((val >> 24) & 0xFF) |
           ((val >> 8) & 0xFF00) |
           ((val << 8) & 0xFF0000) |
           ((val << 24) & 0xFF000000);
}

static float SwapFloat(const uint8_t* ptr) {
    uint32_t raw;
    std::memcpy(&raw, ptr, 4);
    raw = Swap32(raw);
    float f;
    std::memcpy(&f, &raw, 4);
    return f;
}

static int32_t SwapInt(const uint8_t* ptr) {
    uint32_t raw;
    std::memcpy(&raw, ptr, 4);
    raw = Swap32(raw);
    return static_cast<int32_t>(raw);
}

OSCInChanOp::OSCInChanOp(NodeId id, const std::string& name)
    : ChanOp(id, name, "OSCInChanOp") {
    m_outPin = AddOutputPin("output", PinType::Chan);

    SetParam("port", static_cast<int32_t>(7000));
    SetParam("address_filter", std::string(""));
    SetParam("decay_rate", 0.0f);
    SetParam("active", true);
}

OSCInChanOp::~OSCInChanOp() {
    StopListener();
}

void OSCInChanOp::StartListener(int port) {
    StopListener();
    m_running = true;
    m_currentPort = port;
    m_listenerThread = std::thread(&OSCInChanOp::ListenerThreadFunc, this, port);
}

void OSCInChanOp::StopListener() {
    m_running = false;
    if (m_listenerThread.joinable()) {
        m_listenerThread.join();
    }
}

void OSCInChanOp::ProcessRawPacket(const uint8_t* data, size_t size) {
    if (!data || size < 8) return;

    // Address string
    size_t addrEnd = 0;
    while (addrEnd < size && data[addrEnd] != '\0') addrEnd++;
    if (addrEnd >= size || data[0] != '/') return;

    std::string address(reinterpret_cast<const char*>(data), addrEnd);
    size_t offset = (addrEnd + 4) & ~3; // 4-byte align

    if (offset >= size || data[offset] != ',') return;

    // Typetag string
    size_t tagEnd = offset;
    while (tagEnd < size && data[tagEnd] != '\0') tagEnd++;
    if (tagEnd >= size) return;

    std::string typetags(reinterpret_cast<const char*>(data + offset + 1), tagEnd - (offset + 1));
    offset = (tagEnd + 4) & ~3; // 4-byte align

    std::vector<float> values;
    for (char tag : typetags) {
        if (offset + 4 > size) break;
        if (tag == 'f') {
            values.push_back(SwapFloat(data + offset));
            offset += 4;
        } else if (tag == 'i') {
            values.push_back(static_cast<float>(SwapInt(data + offset)));
            offset += 4;
        } else if (tag == 'd') {
            if (offset + 8 <= size) {
                // double
                offset += 8;
            }
        } else if (tag == 's') {
            while (offset < size && data[offset] != '\0') offset++;
            offset = (offset + 4) & ~3;
        }
    }

    InjectMockOSC(address, values);
}

void OSCInChanOp::InjectMockOSC(const std::string& address, const std::vector<float>& args) {
    std::string filter = "";
    if (GetParam("address_filter").Is<std::string>()) {
        filter = GetParam("address_filter").Get<std::string>();
    }

    if (!filter.empty() && address.rfind(filter, 0) != 0) {
        return; // Filter mismatch
    }

    // Convert address to clean channel name (e.g. /light/color -> light_color)
    std::string cleanName = address;
    if (!cleanName.empty() && cleanName[0] == '/') cleanName.erase(0, 1);
    std::replace(cleanName.begin(), cleanName.end(), '/', '_');
    std::replace(cleanName.begin(), cleanName.end(), ' ', '_');

    std::lock_guard<std::mutex> lock(m_valueMutex);
    if (args.empty()) {
        m_channelValues[cleanName] = 1.0f;
    } else if (args.size() == 1) {
        m_channelValues[cleanName] = args[0];
    } else {
        for (size_t i = 0; i < args.size(); ++i) {
            m_channelValues[cleanName + "_" + std::to_string(i + 1)] = args[i];
        }
    }
}

void OSCInChanOp::ListenerThreadFunc(int port) {
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET) {
        m_running = false;
        return;
    }

    int reuse = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&reuse), sizeof(reuse));

#ifdef _WIN32
    DWORD timeout = 100;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&timeout), sizeof(timeout));
#else
    timeval tv{};
    tv.tv_sec = 0;
    tv.tv_usec = 100000;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&tv), sizeof(tv));
#endif

    sockaddr_in bindAddr{};
    bindAddr.sin_family = AF_INET;
    bindAddr.sin_port = htons(static_cast<uint16_t>(port));
    bindAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, reinterpret_cast<sockaddr*>(&bindAddr), sizeof(bindAddr)) < 0) {
        spdlog::warn("OSCInChanOp: Failed to bind UDP port {}", port);
        closesocket(sock);
        m_running = false;
        return;
    }

    uint8_t buffer[2048];
    while (m_running) {
        sockaddr_in srcAddr{};
        int srcLen = sizeof(srcAddr);
        int bytes = recvfrom(sock, reinterpret_cast<char*>(buffer), sizeof(buffer), 0, reinterpret_cast<sockaddr*>(&srcAddr), &srcLen);
        if (bytes > 0) {
            ProcessRawPacket(buffer, static_cast<size_t>(bytes));
        }
    }

    closesocket(sock);
}

bool OSCInChanOp::Cook(const CookContext& /*context*/) {
    bool active = true;
    if (GetParam("active").Is<bool>()) {
        active = GetParam("active").Get<bool>();
    }

    int32_t port = 7000;
    if (GetParam("port").Is<int32_t>()) {
        port = GetParam("port").Get<int32_t>();
    }

    if (active) {
        if (!m_running || m_currentPort != port) {
            StartListener(port);
        }
    } else {
        StopListener();
    }

    float decayRate = 0.0f;
    if (GetParam("decay_rate").Is<float>()) {
        decayRate = GetParam("decay_rate").Get<float>();
    }

    std::vector<std::string> names;
    std::vector<float> values;

    {
        std::lock_guard<std::mutex> lock(m_valueMutex);
        for (auto& [name, val] : m_channelValues) {
            names.push_back(name);
            values.push_back(val);
            if (decayRate > 0.0f) {
                val *= std::max(0.0f, 1.0f - (decayRate / 60.0f));
            }
        }
    }

    if (names.empty()) {
        names.push_back("chan1");
        values.push_back(0.0f);
    }

    ChannelBuffer buf(names, 1, 60.0f);
    for (size_t i = 0; i < names.size(); ++i) {
        float* p = buf.GetChannelData(i);
        if (p) *p = values[i];
    }

    SetOutputBuffer(buf);
    return true;
}

} // namespace nf
