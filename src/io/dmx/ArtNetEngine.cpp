#include "ArtNetEngine.h"
#include <spdlog/spdlog.h>
#include <cstring>
#include <algorithm>

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
#include <arpa/inet.h>
#include <unistd.h>
#define SOCKET int
#define INVALID_SOCKET -1
#define closesocket close
#endif

namespace nf {

ArtNetEngine& ArtNetEngine::Instance() {
    static ArtNetEngine s_instance;
    return s_instance;
}

ArtNetEngine::ArtNetEngine() {
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
}

ArtNetEngine::~ArtNetEngine() {
    StopReceiver();
#ifdef _WIN32
    WSACleanup();
#endif
}

bool ArtNetEngine::StartReceiver(uint16_t port) {
    if (m_receiverRunning) {
        if (m_receiverPort == port) return true;
        StopReceiver();
    }
    m_receiverPort = port;
    m_receiverRunning = true;
    m_receiverThread = std::thread(&ArtNetEngine::ReceiverThreadFunc, this, port);
    return true;
}

void ArtNetEngine::StopReceiver() {
    m_receiverRunning = false;
    if (m_receiverThread.joinable()) {
        m_receiverThread.join();
    }
}

bool ArtNetEngine::IsReceiverRunning() const {
    return m_receiverRunning;
}

bool ArtNetEngine::GetUniverseChannels(uint16_t universe, float* outChannels, size_t count, bool normalized) const {
    if (!outChannels || count == 0) return false;
    std::lock_guard<std::mutex> lock(m_universeMutex);
    auto it = m_universes.find(universe);
    if (it == m_universes.end()) return false;

    size_t copyCount = std::min(count, size_t(512));
    for (size_t i = 0; i < copyCount; ++i) {
        uint8_t byteVal = it->second[i];
        outChannels[i] = normalized ? (static_cast<float>(byteVal) / 255.0f) : static_cast<float>(byteVal);
    }
    for (size_t i = copyCount; i < count; ++i) {
        outChannels[i] = 0.0f;
    }
    return true;
}

bool ArtNetEngine::HasUniverseData(uint16_t universe) const {
    std::lock_guard<std::mutex> lock(m_universeMutex);
    return m_universes.find(universe) != m_universes.end();
}

bool ArtNetEngine::SendDMX(const std::string& host, uint16_t port, uint16_t universe, const float* normalizedChannels, size_t count) {
    if (!normalizedChannels || count == 0) return false;
    uint8_t dmxBytes[512] = {0};
    size_t clampCount = std::min(count, size_t(512));
    for (size_t i = 0; i < clampCount; ++i) {
        float val = std::clamp(normalizedChannels[i], 0.0f, 1.0f);
        dmxBytes[i] = static_cast<uint8_t>(val * 255.0f + 0.5f);
    }
    return SendDMXBytes(host, port, universe, dmxBytes, 512);
}

bool ArtNetEngine::SendDMXBytes(const std::string& host, uint16_t port, uint16_t universe, const uint8_t* dmxBytes, size_t count) {
    if (!dmxBytes || count == 0) return false;

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET) return false;

    // Enable broadcast
    int broadcast = 1;
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, reinterpret_cast<const char*>(&broadcast), sizeof(broadcast));

    sockaddr_in destAddr{};
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(port);
    inet_pton(AF_INET, host.empty() ? "255.255.255.255" : host.c_str(), &destAddr.sin_addr);

    uint8_t seq = 0;
    {
        std::lock_guard<std::mutex> lock(m_universeMutex);
        seq = ++m_sequences[universe];
        if (seq == 0) seq = 1; // 0 means disable sequence in Art-Net
    }

    size_t dmxLen = std::min(count, size_t(512));
    if (dmxLen % 2 != 0) dmxLen++; // Art-Net requires even length

    uint8_t packet[sizeof(ArtDmxHeader) + 512] = {0};
    auto* header = reinterpret_cast<ArtDmxHeader*>(packet);
    std::memcpy(header->id, "Art-Net\0", 8);
    header->opCode = 0x5000; // Little endian: 0x5000 is OpOutput / OpDmx
    header->protVerHi = 0;
    header->protVerLo = 14;
    header->sequence = seq;
    header->physical = 0;
    header->universe = universe;
    header->lengthHi = static_cast<uint8_t>((dmxLen >> 8) & 0xFF);
    header->lengthLo = static_cast<uint8_t>(dmxLen & 0xFF);

    std::memcpy(packet + sizeof(ArtDmxHeader), dmxBytes, std::min(count, size_t(512)));

    int totalSize = static_cast<int>(sizeof(ArtDmxHeader) + dmxLen);
    int sent = sendto(sock, reinterpret_cast<const char*>(packet), totalSize, 0, reinterpret_cast<sockaddr*>(&destAddr), sizeof(destAddr));
    closesocket(sock);

    return sent == totalSize;
}

void ArtNetEngine::InjectMockDMX(uint16_t universe, const uint8_t* dmxBytes, size_t count) {
    if (!dmxBytes || count == 0) return;
    std::lock_guard<std::mutex> lock(m_universeMutex);
    auto& arr = m_universes[universe];
    size_t copyCount = std::min(count, size_t(512));
    std::memcpy(arr.data(), dmxBytes, copyCount);
}

void ArtNetEngine::ClearUniverses() {
    std::lock_guard<std::mutex> lock(m_universeMutex);
    m_universes.clear();
    m_sequences.clear();
}

void ArtNetEngine::ReceiverThreadFunc(uint16_t port) {
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET) {
        m_receiverRunning = false;
        return;
    }

    int reuse = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&reuse), sizeof(reuse));

    // Set non-blocking timeout
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
    bindAddr.sin_port = htons(port);
    bindAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, reinterpret_cast<sockaddr*>(&bindAddr), sizeof(bindAddr)) < 0) {
        spdlog::warn("ArtNetEngine: Failed to bind UDP port {}", port);
        closesocket(sock);
        m_receiverRunning = false;
        return;
    }

    spdlog::info("ArtNetEngine: Receiver listening on UDP port {}", port);

    uint8_t buffer[1024];
    while (m_receiverRunning) {
        sockaddr_in srcAddr{};
        int srcLen = sizeof(srcAddr);
        int bytes = recvfrom(sock, reinterpret_cast<char*>(buffer), sizeof(buffer), 0, reinterpret_cast<sockaddr*>(&srcAddr), &srcLen);
        if (bytes >= static_cast<int>(sizeof(ArtDmxHeader))) {
            const auto* header = reinterpret_cast<const ArtDmxHeader*>(buffer);
            if (std::memcmp(header->id, "Art-Net\0", 8) == 0 && header->opCode == 0x5000) {
                uint16_t universe = header->universe;
                uint16_t length = (static_cast<uint16_t>(header->lengthHi) << 8) | header->lengthLo;
                length = std::min(length, static_cast<uint16_t>(512));

                if (bytes >= static_cast<int>(sizeof(ArtDmxHeader) + length)) {
                    InjectMockDMX(universe, buffer + sizeof(ArtDmxHeader), length);
                }
            }
        }
    }

    closesocket(sock);
}

} // namespace nf
