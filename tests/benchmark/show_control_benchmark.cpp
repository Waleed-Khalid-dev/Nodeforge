#include <gtest/gtest.h>
#include "graph/Graph.h"
#include "io/midi/MidiManager.h"
#include "io/dmx/ArtNetEngine.h"
#include "io/input/InputManager.h"
#include "operators/chan/MIDIInChanOp.h"
#include "operators/chan/OSCInChanOp.h"
#include "operators/data/SerialDataOp.h"
#include "operators/chan/DMXInChanOp.h"
#include "operators/chan/MouseInChanOp.h"
#include "operators/chan/KeyboardInChanOp.h"
#include <chrono>

using namespace nf;

TEST(ShowControlBenchmark, MultiProtocolContinuousStressTest) {
    Graph graph;

    auto* midiIn = graph.CreateNode<MIDIInChanOp>("midi1");
    auto* oscIn = graph.CreateNode<OSCInChanOp>("osc1");
    auto* serialOp = graph.CreateNode<SerialDataOp>("serial1");
    auto* dmxIn = graph.CreateNode<DMXInChanOp>("dmx1");
    auto* mouseIn = graph.CreateNode<MouseInChanOp>("mouse1");
    auto* keyIn = graph.CreateNode<KeyboardInChanOp>("keys1");

    midiIn->SetParam("active", true);
    oscIn->SetParam("active", false);
    serialOp->SetParam("active", false);
    dmxIn->SetParam("active", true);
    mouseIn->SetParam("active", true);
    keyIn->SetParam("active", true);

    auto& midi = MidiManager::Instance();
    auto& artnet = ArtNetEngine::Instance();
    auto& input = InputManager::Instance();

    CookContext ctx{ .frameIndex = 0, .timeSeconds = 0.0, .deltaTimeSeconds = 1.0 / 60.0 };
    const int totalFrames = 2000;
    auto startTime = std::chrono::high_resolution_clock::now();

    for (int frame = 0; frame < totalFrames; ++frame) {
        // High-frequency injection across protocols
        MidiEvent ev;
        ev.channel = 1;
        ev.type = MidiEventType::ControlChange;
        ev.data1 = static_cast<uint8_t>(frame % 128);
        ev.data2 = static_cast<uint8_t>((frame * 7) % 128);
        midi.InjectVirtualEvent(ev);

        uint8_t dmxData[512] = {0};
        dmxData[0] = static_cast<uint8_t>(frame % 256);
        artnet.InjectMockDMX(0, dmxData, 512);

        input.InjectMockMouse(static_cast<float>(frame % 100) / 100.0f, 0.5f, (frame % 2 == 0));
        input.InjectMockKey("space", (frame % 3 == 0));

        ctx.frameIndex = frame;
        ctx.timeSeconds = frame / 60.0;
        graph.CookAll(ctx);
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    double totalMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    double avgMsPerCook = totalMs / totalFrames;

    // In Debug build with full diagnostics, all 6 protocols combined must evaluate in < 5.0 ms (<16.6ms 60 FPS budget)
    EXPECT_LT(avgMsPerCook, 5.0);
}
