#include "MIDIInChanOp.h"
#include "io/midi/MidiManager.h"
#include <algorithm>

namespace nf {

struct MidiNameTables {
    std::array<std::array<std::string, 128>, 16> ccNames;
    std::array<std::array<std::string, 128>, 16> noteNames;
    std::array<std::string, 16> pitchNames;

    MidiNameTables() {
        for (int c = 0; c < 16; ++c) {
            pitchNames[c] = "ch" + std::to_string(c + 1) + "_pitch";
            for (int i = 0; i < 128; ++i) {
                ccNames[c][i] = "ch" + std::to_string(c + 1) + "_cc" + std::to_string(i);
                noteNames[c][i] = "ch" + std::to_string(c + 1) + "_n" + std::to_string(i);
            }
        }
    }
};

static const MidiNameTables& GetMidiNameTables() {
    static const MidiNameTables s_tables;
    return s_tables;
}

MIDIInChanOp::MIDIInChanOp(NodeId id, const std::string& name)
    : ChanOp(id, name, "MIDIInChanOp") {
    m_outPin = AddOutputPin("output", PinType::Chan);

    SetParam("device_index", static_cast<int32_t>(0));
    SetParam("channel_filter", static_cast<int32_t>(0)); // 0 = all
    SetParam("normalized", true);
    SetParam("include_notes", true);
    SetParam("include_cc", true);
    SetParam("include_pitchbend", true);
    SetParam("active", true);
}

MIDIInChanOp::~MIDIInChanOp() {
    if (m_currentDevice >= 0) {
        MidiManager::Instance().CloseInput(static_cast<uint32_t>(m_currentDevice));
    }
}

bool MIDIInChanOp::Cook(const CookContext& /*context*/) {
    bool active = true;
    if (GetParam("active").Is<bool>()) {
        active = GetParam("active").Get<bool>();
    }

    int32_t devIdx = 0;
    if (GetParam("device_index").Is<int32_t>()) {
        devIdx = std::max(0, GetParam("device_index").Get<int32_t>());
    }

    if (active) {
        if (m_currentDevice != devIdx) {
            if (m_currentDevice >= 0) {
                MidiManager::Instance().CloseInput(static_cast<uint32_t>(m_currentDevice));
            }
            m_currentDevice = devIdx;
            MidiManager::Instance().OpenInput(static_cast<uint32_t>(m_currentDevice));
        }
    } else {
        if (m_currentDevice >= 0) {
            MidiManager::Instance().CloseInput(static_cast<uint32_t>(m_currentDevice));
            m_currentDevice = -1;
        }
    }

    int32_t chanFilter = 0;
    if (GetParam("channel_filter").Is<int32_t>()) {
        chanFilter = std::clamp(GetParam("channel_filter").Get<int32_t>(), 0, 16);
    }

    bool normalized = true;
    if (GetParam("normalized").Is<bool>()) {
        normalized = GetParam("normalized").Get<bool>();
    }

    bool incNotes = true;
    if (GetParam("include_notes").Is<bool>()) {
        incNotes = GetParam("include_notes").Get<bool>();
    }

    bool incCC = true;
    if (GetParam("include_cc").Is<bool>()) {
        incCC = GetParam("include_cc").Get<bool>();
    }

    bool incPitch = true;
    if (GetParam("include_pitchbend").Is<bool>()) {
        incPitch = GetParam("include_pitchbend").Get<bool>();
    }

    auto snap = MidiManager::Instance().GetSnapshot();
    const auto& namesTable = GetMidiNameTables();

    std::vector<std::string> names;
    std::vector<float> values;

    int startChan = (chanFilter == 0) ? 1 : chanFilter;
    int endChan = (chanFilter == 0) ? 16 : chanFilter;

    // Collect active CCs and notes
    for (int c = startChan; c <= endChan; ++c) {
        size_t cIdx = static_cast<size_t>(c - 1);

        if (incCC) {
            for (int cc = 0; cc < 128; ++cc) {
                float val = snap.ccState[cIdx][cc];
                if (!normalized) val *= 127.0f;
                if (val > 0.0f || cc < 8) { // Always expose first 8 CCs or non-zero
                    names.push_back(namesTable.ccNames[cIdx][cc]);
                    values.push_back(val);
                }
            }
        }

        if (incNotes) {
            for (int n = 0; n < 128; ++n) {
                float val = snap.noteState[cIdx][n];
                if (!normalized) val *= 127.0f;
                if (val > 0.0f) {
                    names.push_back(namesTable.noteNames[cIdx][n]);
                    values.push_back(val);
                }
            }
        }

        if (incPitch) {
            names.push_back(namesTable.pitchNames[cIdx]);
            float pVal = snap.pitchBendState[cIdx];
            if (!normalized) pVal = (pVal - 0.5f) * 2.0f;
            values.push_back(pVal);
        }
    }

    if (names.empty()) {
        names.push_back("ch1_cc1");
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
