#include "MidiManager.h"
#include <spdlog/spdlog.h>
#include <chrono>
#include <algorithm>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#endif

namespace nf {

struct MidiManager::Impl {
    mutable std::mutex stateMutex;
    mutable std::mutex eventMutex;

    // 16 channels x 128 CCs
    std::array<std::array<float, 128>, 16> ccState{};
    // 16 channels x 128 Notes (0.0 = off, >0.0 = velocity)
    std::array<std::array<float, 128>, 16> noteState{};
    // 16 channels PitchBend (-1.0 to 1.0 or normalized 0.0 to 1.0)
    std::array<float, 16> pitchBendState{};
    // 16 channels Aftertouch
    std::array<float, 16> aftertouchState{};

    std::deque<MidiEvent> eventQueue;

#ifdef _WIN32
    std::vector<HMIDIIN> openInputs;
    std::vector<HMIDIOUT> openOutputs;
#endif

    Impl() {
        for (auto& arr : ccState) arr.fill(0.0f);
        for (auto& arr : noteState) arr.fill(0.0f);
        pitchBendState.fill(0.5f); // Centered
        aftertouchState.fill(0.0f);
    }

    ~Impl() {
#ifdef _WIN32
        for (auto h : openInputs) {
            if (h) {
                midiInStop(h);
                midiInReset(h);
                midiInClose(h);
            }
        }
        for (auto h : openOutputs) {
            if (h) {
                midiOutReset(h);
                midiOutClose(h);
            }
        }
#endif
    }

    void ProcessRawMidi(DWORD dwParam1, [[maybe_unused]] DWORD dwParam2) {
        uint8_t status = static_cast<uint8_t>(dwParam1 & 0xFF);
        uint8_t data1 = static_cast<uint8_t>((dwParam1 >> 8) & 0xFF);
        uint8_t data2 = static_cast<uint8_t>((dwParam1 >> 16) & 0xFF);

        uint8_t msgType = status & 0xF0;
        uint8_t channel = static_cast<uint8_t>((status & 0x0F) + 1); // 1-16

        MidiEvent ev;
        ev.channel = channel;
        ev.data1 = data1;
        ev.data2 = data2;

        auto now = std::chrono::high_resolution_clock::now();
        ev.timestamp = std::chrono::duration<double>(now.time_since_epoch()).count();

        {
            std::lock_guard<std::mutex> lock(stateMutex);
            size_t cIdx = std::clamp(static_cast<size_t>(channel - 1), size_t(0), size_t(15));

            switch (msgType) {
            case 0x80: // Note Off
                ev.type = MidiEventType::NoteOff;
                ev.normalizedValue = 0.0f;
                if (data1 < 128) noteState[cIdx][data1] = 0.0f;
                break;
            case 0x90: // Note On
                if (data2 == 0) {
                    ev.type = MidiEventType::NoteOff;
                    ev.normalizedValue = 0.0f;
                    if (data1 < 128) noteState[cIdx][data1] = 0.0f;
                } else {
                    ev.type = MidiEventType::NoteOn;
                    ev.normalizedValue = static_cast<float>(data2) / 127.0f;
                    if (data1 < 128) noteState[cIdx][data1] = ev.normalizedValue;
                }
                break;
            case 0xA0: // Poly Aftertouch
                ev.type = MidiEventType::PolyAftertouch;
                ev.normalizedValue = static_cast<float>(data2) / 127.0f;
                break;
            case 0xB0: // CC
                ev.type = MidiEventType::ControlChange;
                ev.normalizedValue = static_cast<float>(data2) / 127.0f;
                if (data1 < 128) ccState[cIdx][data1] = ev.normalizedValue;
                break;
            case 0xC0: // Program Change
                ev.type = MidiEventType::ProgramChange;
                ev.normalizedValue = static_cast<float>(data1) / 127.0f;
                break;
            case 0xD0: // Channel Pressure (Aftertouch)
                ev.type = MidiEventType::ChannelPressure;
                ev.normalizedValue = static_cast<float>(data1) / 127.0f;
                aftertouchState[cIdx] = ev.normalizedValue;
                break;
            case 0xE0: { // Pitch Bend (14-bit: data1 = LSB, data2 = MSB)
                ev.type = MidiEventType::PitchBend;
                uint16_t bend = (static_cast<uint16_t>(data2) << 7) | (data1 & 0x7F);
                ev.normalizedValue = static_cast<float>(bend) / 16383.0f;
                pitchBendState[cIdx] = ev.normalizedValue;
                break;
            }
            default:
                ev.type = MidiEventType::Unknown;
                break;
            }
        }

        {
            std::lock_guard<std::mutex> lock(eventMutex);
            if (eventQueue.size() > 2048) {
                eventQueue.pop_front();
            }
            eventQueue.push_back(ev);
        }
    }
};

#ifdef _WIN32
static void CALLBACK MidiInCallback(HMIDIIN /*hMidiIn*/, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
    if (wMsg == MIM_DATA) {
        auto* impl = reinterpret_cast<MidiManager::Impl*>(dwInstance);
        if (impl) {
            impl->ProcessRawMidi(static_cast<DWORD>(dwParam1), static_cast<DWORD>(dwParam2));
        }
    }
}
#endif

MidiManager& MidiManager::Instance() {
    static MidiManager s_instance;
    return s_instance;
}

MidiManager::MidiManager()
    : m_impl(std::make_unique<Impl>()) {
}

MidiManager::~MidiManager() = default;

uint32_t MidiManager::GetInputDeviceCount() const {
#ifdef _WIN32
    return midiInGetNumDevs();
#else
    return 0;
#endif
}

std::string MidiManager::GetInputDeviceName(uint32_t index) const {
#ifdef _WIN32
    MIDIINCAPSA caps;
    if (midiInGetDevCapsA(index, &caps, sizeof(caps)) == MMSYSERR_NOERROR) {
        return std::string(caps.szPname);
    }
#endif
    return "Virtual MIDI In " + std::to_string(index);
}

uint32_t MidiManager::GetOutputDeviceCount() const {
#ifdef _WIN32
    return midiOutGetNumDevs();
#else
    return 0;
#endif
}

std::string MidiManager::GetOutputDeviceName(uint32_t index) const {
#ifdef _WIN32
    MIDIOUTCAPSA caps;
    if (midiOutGetDevCapsA(index, &caps, sizeof(caps)) == MMSYSERR_NOERROR) {
        return std::string(caps.szPname);
    }
#endif
    return "Virtual MIDI Out " + std::to_string(index);
}

bool MidiManager::OpenInput(uint32_t index) {
#ifdef _WIN32
    if (index >= GetInputDeviceCount()) return false;
    if (m_impl->openInputs.size() <= index) {
        m_impl->openInputs.resize(index + 1, nullptr);
    }
    if (m_impl->openInputs[index] != nullptr) return true;

    HMIDIIN hIn = nullptr;
    MMRESULT res = midiInOpen(&hIn, index, reinterpret_cast<DWORD_PTR>(MidiInCallback), reinterpret_cast<DWORD_PTR>(m_impl.get()), CALLBACK_FUNCTION);
    if (res == MMSYSERR_NOERROR && hIn) {
        midiInStart(hIn);
        m_impl->openInputs[index] = hIn;
        spdlog::info("MidiManager: Opened input port {} ({})", index, GetInputDeviceName(index));
        return true;
    }
    spdlog::warn("MidiManager: Failed to open input port {}", index);
#else
    (void)index;
#endif
    return false;
}

void MidiManager::CloseInput(uint32_t index) {
#ifdef _WIN32
    if (index < m_impl->openInputs.size() && m_impl->openInputs[index]) {
        midiInStop(m_impl->openInputs[index]);
        midiInReset(m_impl->openInputs[index]);
        midiInClose(m_impl->openInputs[index]);
        m_impl->openInputs[index] = nullptr;
    }
#else
    (void)index;
#endif
}

bool MidiManager::IsInputOpen(uint32_t index) const {
#ifdef _WIN32
    if (index < m_impl->openInputs.size()) {
        return m_impl->openInputs[index] != nullptr;
    }
#else
    (void)index;
#endif
    return false;
}

bool MidiManager::OpenOutput(uint32_t index) {
#ifdef _WIN32
    if (index >= GetOutputDeviceCount()) return false;
    if (m_impl->openOutputs.size() <= index) {
        m_impl->openOutputs.resize(index + 1, nullptr);
    }
    if (m_impl->openOutputs[index] != nullptr) return true;

    HMIDIOUT hOut = nullptr;
    MMRESULT res = midiOutOpen(&hOut, index, 0, 0, CALLBACK_NULL);
    if (res == MMSYSERR_NOERROR && hOut) {
        m_impl->openOutputs[index] = hOut;
        spdlog::info("MidiManager: Opened output port {} ({})", index, GetOutputDeviceName(index));
        return true;
    }
#else
    (void)index;
#endif
    return false;
}

void MidiManager::CloseOutput(uint32_t index) {
#ifdef _WIN32
    if (index < m_impl->openOutputs.size() && m_impl->openOutputs[index]) {
        midiOutReset(m_impl->openOutputs[index]);
        midiOutClose(m_impl->openOutputs[index]);
        m_impl->openOutputs[index] = nullptr;
    }
#else
    (void)index;
#endif
}

bool MidiManager::IsOutputOpen(uint32_t index) const {
#ifdef _WIN32
    if (index < m_impl->openOutputs.size()) {
        return m_impl->openOutputs[index] != nullptr;
    }
#else
    (void)index;
#endif
    return false;
}

bool MidiManager::SendShortMsg(uint32_t deviceIndex, uint8_t status, uint8_t data1, uint8_t data2) {
#ifdef _WIN32
    if (deviceIndex < m_impl->openOutputs.size() && m_impl->openOutputs[deviceIndex]) {
        DWORD msg = (static_cast<DWORD>(data2) << 16) | (static_cast<DWORD>(data1) << 8) | status;
        return midiOutShortMsg(m_impl->openOutputs[deviceIndex], msg) == MMSYSERR_NOERROR;
    }
#else
    (void)deviceIndex; (void)status; (void)data1; (void)data2;
#endif
    return false;
}

bool MidiManager::SendNoteOn(uint32_t deviceIndex, uint8_t channel, uint8_t note, uint8_t velocity) {
    uint8_t c = static_cast<uint8_t>(std::clamp(static_cast<int>(channel), 1, 16) - 1);
    return SendShortMsg(deviceIndex, static_cast<uint8_t>(0x90 | c), note & 0x7F, velocity & 0x7F);
}

bool MidiManager::SendNoteOff(uint32_t deviceIndex, uint8_t channel, uint8_t note, uint8_t velocity) {
    uint8_t c = static_cast<uint8_t>(std::clamp(static_cast<int>(channel), 1, 16) - 1);
    return SendShortMsg(deviceIndex, static_cast<uint8_t>(0x80 | c), note & 0x7F, velocity & 0x7F);
}

bool MidiManager::SendControlChange(uint32_t deviceIndex, uint8_t channel, uint8_t cc, uint8_t value) {
    uint8_t c = static_cast<uint8_t>(std::clamp(static_cast<int>(channel), 1, 16) - 1);
    return SendShortMsg(deviceIndex, static_cast<uint8_t>(0xB0 | c), cc & 0x7F, value & 0x7F);
}

bool MidiManager::SendPitchBend(uint32_t deviceIndex, uint8_t channel, int16_t bend) {
    uint8_t c = static_cast<uint8_t>(std::clamp(static_cast<int>(channel), 1, 16) - 1);
    int clamped = std::clamp(static_cast<int>(bend), -8192, 8191) + 8192;
    uint8_t lsb = static_cast<uint8_t>(clamped & 0x7F);
    uint8_t msb = static_cast<uint8_t>((clamped >> 7) & 0x7F);
    return SendShortMsg(deviceIndex, static_cast<uint8_t>(0xE0 | c), lsb, msb);
}

MidiManager::Snapshot MidiManager::GetSnapshot() const {
    std::lock_guard<std::mutex> lock(m_impl->stateMutex);
    Snapshot snap;
    snap.ccState = m_impl->ccState;
    snap.noteState = m_impl->noteState;
    snap.pitchBendState = m_impl->pitchBendState;
    snap.aftertouchState = m_impl->aftertouchState;
    return snap;
}

float MidiManager::GetCCValue(uint8_t channel, uint8_t cc, bool normalized) const {
    if (channel < 1 || channel > 16 || cc > 127) return 0.0f;
    std::lock_guard<std::mutex> lock(m_impl->stateMutex);
    float val = m_impl->ccState[channel - 1][cc];
    return normalized ? val : (val * 127.0f);
}

float MidiManager::GetNoteValue(uint8_t channel, uint8_t note, bool normalized) const {
    if (channel < 1 || channel > 16 || note > 127) return 0.0f;
    std::lock_guard<std::mutex> lock(m_impl->stateMutex);
    float val = m_impl->noteState[channel - 1][note];
    return normalized ? val : (val * 127.0f);
}

float MidiManager::GetPitchBend(uint8_t channel, bool normalized) const {
    if (channel < 1 || channel > 16) return 0.5f;
    std::lock_guard<std::mutex> lock(m_impl->stateMutex);
    float val = m_impl->pitchBendState[channel - 1];
    return normalized ? val : ((val - 0.5f) * 2.0f);
}

float MidiManager::GetAftertouch(uint8_t channel, bool normalized) const {
    if (channel < 1 || channel > 16) return 0.0f;
    std::lock_guard<std::mutex> lock(m_impl->stateMutex);
    float val = m_impl->aftertouchState[channel - 1];
    return normalized ? val : (val * 127.0f);
}

std::vector<MidiEvent> MidiManager::DrainEvents() {
    std::lock_guard<std::mutex> lock(m_impl->eventMutex);
    std::vector<MidiEvent> events(m_impl->eventQueue.begin(), m_impl->eventQueue.end());
    m_impl->eventQueue.clear();
    return events;
}

void MidiManager::InjectVirtualEvent(const MidiEvent& ev) {
    uint8_t status = static_cast<uint8_t>(static_cast<uint8_t>(ev.type) | (std::clamp(static_cast<int>(ev.channel), 1, 16) - 1));
    DWORD raw = (static_cast<DWORD>(ev.data2) << 16) | (static_cast<DWORD>(ev.data1) << 8) | status;
    m_impl->ProcessRawMidi(raw, 0);
}

void MidiManager::ClearState() {
    std::lock_guard<std::mutex> lock(m_impl->stateMutex);
    for (auto& arr : m_impl->ccState) arr.fill(0.0f);
    for (auto& arr : m_impl->noteState) arr.fill(0.0f);
    m_impl->pitchBendState.fill(0.5f);
    m_impl->aftertouchState.fill(0.0f);

    std::lock_guard<std::mutex> eventLock(m_impl->eventMutex);
    m_impl->eventQueue.clear();
}

} // namespace nf
