#include "MIDIOutChanOp.h"
#include "io/midi/MidiManager.h"
#include <algorithm>
#include <cmath>

namespace nf {

MIDIOutChanOp::MIDIOutChanOp(NodeId id, const std::string& name)
    : ChanOp(id, name, "MIDIOutChanOp") {
    AddInputPin("input", PinType::Chan);
    m_outPin = AddOutputPin("output", PinType::Chan);

    SetParam("device_index", static_cast<int32_t>(0));
    SetParam("midi_channel", static_cast<int32_t>(1));
    SetParam("mode", static_cast<int32_t>(0)); // 0=Auto, 1=CC, 2=Note
    SetParam("cc_number", static_cast<int32_t>(1));
    SetParam("threshold", 0.5f);
    SetParam("active", true);
}

MIDIOutChanOp::~MIDIOutChanOp() {
    if (m_currentDevice >= 0) {
        MidiManager::Instance().CloseOutput(static_cast<uint32_t>(m_currentDevice));
    }
}

bool MIDIOutChanOp::Cook(const CookContext& /*context*/) {
    Pin* inPin = GetInputPin("input");
    ChannelBuffer inBuf;
    if (inPin && inPin->IsConnected()) {
        inBuf = inPin->GetValue().Get<ChannelBuffer>();
    }
    SetOutputBuffer(inBuf);

    bool active = true;
    if (GetParam("active").Is<bool>()) {
        active = GetParam("active").Get<bool>();
    }
    if (!active || inBuf.GetChannelCount() == 0) {
        return true;
    }

    int32_t devIdx = 0;
    if (GetParam("device_index").Is<int32_t>()) {
        devIdx = std::max(0, GetParam("device_index").Get<int32_t>());
    }

    if (m_currentDevice != devIdx) {
        if (m_currentDevice >= 0) {
            MidiManager::Instance().CloseOutput(static_cast<uint32_t>(m_currentDevice));
        }
        m_currentDevice = devIdx;
        MidiManager::Instance().OpenOutput(static_cast<uint32_t>(m_currentDevice));
    }

    int32_t midiChan = 1;
    if (GetParam("midi_channel").Is<int32_t>()) {
        midiChan = std::clamp(GetParam("midi_channel").Get<int32_t>(), 1, 16);
    }

    int32_t mode = 0;
    if (GetParam("mode").Is<int32_t>()) {
        mode = std::clamp(GetParam("mode").Get<int32_t>(), 0, 2);
    }

    int32_t defaultCC = 1;
    if (GetParam("cc_number").Is<int32_t>()) {
        defaultCC = std::clamp(GetParam("cc_number").Get<int32_t>(), 0, 127);
    }

    float threshold = 0.5f;
    if (GetParam("threshold").Is<float>()) {
        threshold = GetParam("threshold").Get<float>();
    }

    auto& midi = MidiManager::Instance();

    for (size_t i = 0; i < inBuf.GetChannelCount(); ++i) {
        const std::string& name = inBuf.GetChannelNames()[i];
        const float* data = inBuf.GetChannelData(i);
        if (!data) continue;
        float currentVal = data[inBuf.GetSampleCount() - 1]; // Latest sample
        float prevVal = m_prevValues[name];

        if (mode == 1) { // Force CC
            uint8_t ccVal = static_cast<uint8_t>(std::clamp(currentVal, 0.0f, 1.0f) * 127.0f);
            if (std::abs(currentVal - prevVal) > 0.005f) {
                midi.SendControlChange(static_cast<uint32_t>(devIdx), static_cast<uint8_t>(midiChan), static_cast<uint8_t>(defaultCC + i), ccVal);
            }
        } else if (mode == 2) { // Force Note Trigger
            bool isNoteOn = currentVal >= threshold;
            bool wasNoteOn = m_noteState[name];
            uint8_t noteNum = static_cast<uint8_t>(std::clamp(60 + static_cast<int>(i), 0, 127));

            if (isNoteOn && !wasNoteOn) {
                uint8_t vel = static_cast<uint8_t>(std::clamp(currentVal, 0.0f, 1.0f) * 127.0f);
                if (vel == 0) vel = 100;
                midi.SendNoteOn(static_cast<uint32_t>(devIdx), static_cast<uint8_t>(midiChan), noteNum, vel);
            } else if (!isNoteOn && wasNoteOn) {
                midi.SendNoteOff(static_cast<uint32_t>(devIdx), static_cast<uint8_t>(midiChan), noteNum, 0);
            }
            m_noteState[name] = isNoteOn;
        } else { // Auto-detect from channel name
            if (name.find("_cc") != std::string::npos || name.find("cc") == 0) {
                size_t pos = name.rfind("cc");
                int cc = std::atoi(name.c_str() + pos + 2);
                uint8_t ccVal = static_cast<uint8_t>(std::clamp(currentVal, 0.0f, 1.0f) * 127.0f);
                if (std::abs(currentVal - prevVal) > 0.005f) {
                    midi.SendControlChange(static_cast<uint32_t>(devIdx), static_cast<uint8_t>(midiChan), static_cast<uint8_t>(cc), ccVal);
                }
            } else if (name.find("_n") != std::string::npos || name.find("note") != std::string::npos) {
                size_t pos = name.rfind('n');
                int note = std::atoi(name.c_str() + pos + 1);
                bool isNoteOn = currentVal >= threshold;
                bool wasNoteOn = m_noteState[name];
                if (isNoteOn && !wasNoteOn) {
                    uint8_t vel = static_cast<uint8_t>(std::clamp(currentVal, 0.0f, 1.0f) * 127.0f);
                    if (vel == 0) vel = 100;
                    midi.SendNoteOn(static_cast<uint32_t>(devIdx), static_cast<uint8_t>(midiChan), static_cast<uint8_t>(note), vel);
                } else if (!isNoteOn && wasNoteOn) {
                    midi.SendNoteOff(static_cast<uint32_t>(devIdx), static_cast<uint8_t>(midiChan), static_cast<uint8_t>(note), 0);
                }
                m_noteState[name] = isNoteOn;
            }
        }

        m_prevValues[name] = currentVal;
    }

    return true;
}

} // namespace nf
