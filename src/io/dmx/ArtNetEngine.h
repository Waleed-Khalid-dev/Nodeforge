#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <thread>
#include <memory>

namespace nf {

#pragma pack(push, 1)
struct ArtDmxHeader {
    char id[8];          // "Art-Net\0"
    uint16_t opCode;     // 0x5000 (OpDmx)
    uint8_t protVerHi;   // 0
    uint8_t protVerLo;   // 14
    uint8_t sequence;    // 0 - 255
    uint8_t physical;    // 0
    uint16_t universe;   // 0 - 32767
    uint8_t lengthHi;    // length MSB
    uint8_t lengthLo;    // length LSB
};
#pragma pack(pop)

class ArtNetEngine {
public:
    static ArtNetEngine& Instance();

    ArtNetEngine();
    ~ArtNetEngine();

    // Receiver listener
    bool StartReceiver(uint16_t port = 6454);
    void StopReceiver();
    bool IsReceiverRunning() const;

    // Retrieve latest received universe channel data (normalized 0.0..1.0 or raw 0..255)
    bool GetUniverseChannels(uint16_t universe, float* outChannels, size_t count, bool normalized = true) const;
    bool HasUniverseData(uint16_t universe) const;

    // Transmitter
    bool SendDMX(const std::string& host, uint16_t port, uint16_t universe, const float* normalizedChannels, size_t count);
    bool SendDMXBytes(const std::string& host, uint16_t port, uint16_t universe, const uint8_t* dmxBytes, size_t count);

    // Testing simulation
    void InjectMockDMX(uint16_t universe, const uint8_t* dmxBytes, size_t count);
    void ClearUniverses();

private:
    void ReceiverThreadFunc(uint16_t port);

    std::atomic<bool> m_receiverRunning{false};
    uint16_t m_receiverPort{6454};
    std::thread m_receiverThread;

    mutable std::mutex m_universeMutex;
    std::unordered_map<uint16_t, std::array<uint8_t, 512>> m_universes;
    std::unordered_map<uint16_t, uint8_t> m_sequences;
};

} // namespace nf
