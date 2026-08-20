#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include <mutex>
#include <atomic>
#include <deque>
#include <memory>

namespace nf {

enum class MidiEventType : uint8_t {
    NoteOff = 0x80,
    NoteOn = 0x90,
    PolyAftertouch = 0xA0,
    ControlChange = 0xB0,
    ProgramChange = 0xC0,
    ChannelPressure = 0xD0,
    PitchBend = 0xE0,
    Unknown = 0x00
};

struct MidiEvent {
    uint8_t channel = 1;       // 1 - 16
    MidiEventType type = MidiEventType::Unknown;
    uint8_t data1 = 0;         // Note number or CC index (0-127)
    uint8_t data2 = 0;         // Velocity or CC value (0-127)
    float normalizedValue = 0.0f; // 0.0 - 1.0
    double timestamp = 0.0;
};

class MidiManager {
public:
    static MidiManager& Instance();

    MidiManager();
    ~MidiManager();

    // Device enumeration
    uint32_t GetInputDeviceCount() const;
    std::string GetInputDeviceName(uint32_t index) const;
    uint32_t GetOutputDeviceCount() const;
    std::string GetOutputDeviceName(uint32_t index) const;

    // Device connection
    bool OpenInput(uint32_t index);
    void CloseInput(uint32_t index);
    bool IsInputOpen(uint32_t index) const;

    bool OpenOutput(uint32_t index);
    void CloseOutput(uint32_t index);
    bool IsOutputOpen(uint32_t index) const;

    // Transmission
    bool SendShortMsg(uint32_t deviceIndex, uint8_t status, uint8_t data1, uint8_t data2);
    bool SendNoteOn(uint32_t deviceIndex, uint8_t channel, uint8_t note, uint8_t velocity);
    bool SendNoteOff(uint32_t deviceIndex, uint8_t channel, uint8_t note, uint8_t velocity);
    bool SendControlChange(uint32_t deviceIndex, uint8_t channel, uint8_t cc, uint8_t value);
    bool SendPitchBend(uint32_t deviceIndex, uint8_t channel, int16_t bend); // -8192 to 8191

    // Snapshot for zero-lock fast cook
    struct Snapshot {
        std::array<std::array<float, 128>, 16> ccState;
        std::array<std::array<float, 128>, 16> noteState;
        std::array<float, 16> pitchBendState;
        std::array<float, 16> aftertouchState;
    };
    Snapshot GetSnapshot() const;

    // State Inspection (1-based channel 1..16, 0..127 CC/Note)
    float GetCCValue(uint8_t channel, uint8_t cc, bool normalized = true) const;
    float GetNoteValue(uint8_t channel, uint8_t note, bool normalized = true) const;
    float GetPitchBend(uint8_t channel, bool normalized = true) const;
    float GetAftertouch(uint8_t channel, bool normalized = true) const;

    // Event Queue (Drain for per-frame processing)
    std::vector<MidiEvent> DrainEvents();
    
    // Testing / Simulation injection
    void InjectVirtualEvent(const MidiEvent& ev);
    void ClearState();

    struct Impl;
private:
    std::unique_ptr<Impl> m_impl;
};

} // namespace nf
