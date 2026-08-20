#include <gtest/gtest.h>
#include "io/midi/MidiManager.h"
#include "operators/chan/MIDIInChanOp.h"
#include "operators/chan/MIDIOutChanOp.h"
#include "graph/Graph.h"

using namespace nf;

TEST(MidiProtocolTest, MidiManagerVirtualEventInjection) {
    auto& midi = MidiManager::Instance();
    midi.ClearState();

    // Test Control Change (Channel 1, CC 7 (Volume), Value 127)
    MidiEvent ccEvent;
    ccEvent.channel = 1;
    ccEvent.type = MidiEventType::ControlChange;
    ccEvent.data1 = 7;
    ccEvent.data2 = 127;
    midi.InjectVirtualEvent(ccEvent);

    EXPECT_FLOAT_EQ(midi.GetCCValue(1, 7, true), 1.0f);
    EXPECT_FLOAT_EQ(midi.GetCCValue(1, 7, false), 127.0f);

    // Test Note On / Off (Channel 2, Note 60 (Middle C), Velocity 64)
    MidiEvent noteOn;
    noteOn.channel = 2;
    noteOn.type = MidiEventType::NoteOn;
    noteOn.data1 = 60;
    noteOn.data2 = 64;
    midi.InjectVirtualEvent(noteOn);

    EXPECT_NEAR(midi.GetNoteValue(2, 60, true), 64.0f / 127.0f, 0.01f);

    MidiEvent noteOff;
    noteOff.channel = 2;
    noteOff.type = MidiEventType::NoteOff;
    noteOff.data1 = 60;
    noteOff.data2 = 0;
    midi.InjectVirtualEvent(noteOff);

    EXPECT_FLOAT_EQ(midi.GetNoteValue(2, 60, true), 0.0f);

    // Test Pitch Bend
    MidiEvent pitchEv;
    pitchEv.channel = 1;
    pitchEv.type = MidiEventType::PitchBend;
    pitchEv.data1 = 0x00; // LSB
    pitchEv.data2 = 0x7F; // MSB (Max bend)
    midi.InjectVirtualEvent(pitchEv);

    EXPECT_GT(midi.GetPitchBend(1, true), 0.99f);

    auto events = midi.DrainEvents();
    EXPECT_GE(events.size(), 3u);
}

TEST(MidiProtocolTest, MIDIInChanOpCook) {
    auto& midi = MidiManager::Instance();
    midi.ClearState();

    MidiEvent cc1;
    cc1.channel = 1;
    cc1.type = MidiEventType::ControlChange;
    cc1.data1 = 1;
    cc1.data2 = 100;
    midi.InjectVirtualEvent(cc1);

    MidiEvent n60;
    n60.channel = 1;
    n60.type = MidiEventType::NoteOn;
    n60.data1 = 60;
    n60.data2 = 120;
    midi.InjectVirtualEvent(n60);

    MIDIInChanOp midiIn(1, "midi_in1");
    midiIn.SetParam("channel_filter", static_cast<int32_t>(1));
    midiIn.SetParam("normalized", true);

    CookContext ctx;
    EXPECT_TRUE(midiIn.Cook(ctx));

    Pin* outPin = midiIn.GetOutputPin("output");
    ASSERT_NE(outPin, nullptr);
    ChannelBuffer buf = outPin->GetValue().Get<ChannelBuffer>();

    EXPECT_GE(buf.GetChannelCount(), 2u);
    EXPECT_TRUE(buf.HasChannel("ch1_cc1"));
    EXPECT_TRUE(buf.HasChannel("ch1_n60"));

    const float* ccData = buf.GetChannelData("ch1_cc1");
    ASSERT_NE(ccData, nullptr);
    EXPECT_NEAR(*ccData, 100.0f / 127.0f, 0.01f);

    const float* nData = buf.GetChannelData("ch1_n60");
    ASSERT_NE(nData, nullptr);
    EXPECT_NEAR(*nData, 120.0f / 127.0f, 0.01f);
}

TEST(MidiProtocolTest, MIDIOutChanOpPassthroughAndCook) {
    Graph graph;
    auto* outNode = graph.AddNode(std::make_unique<MIDIOutChanOp>(2, "midi_out1"));
    ASSERT_NE(outNode, nullptr);

    outNode->SetParam("active", true);
    outNode->SetParam("mode", static_cast<int32_t>(1)); // Continuous CC
    outNode->SetParam("cc_number", static_cast<int32_t>(10));

    CookContext ctx;
    EXPECT_TRUE(outNode->Cook(ctx));
}
