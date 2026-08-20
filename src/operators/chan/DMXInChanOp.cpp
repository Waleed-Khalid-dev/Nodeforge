#include "DMXInChanOp.h"
#include "io/dmx/ArtNetEngine.h"
#include <algorithm>

namespace nf {

static const std::vector<std::string>& GetDMXChannelNames() {
    static const std::vector<std::string> s_allNames = []() {
        std::vector<std::string> names(512);
        for (int i = 0; i < 512; ++i) {
            names[i] = "ch" + std::to_string(i + 1);
        }
        return names;
    }();
    return s_allNames;
}

DMXInChanOp::DMXInChanOp(NodeId id, const std::string& name)
    : ChanOp(id, name, "DMXInChanOp") {
    m_outPin = AddOutputPin("output", PinType::Chan);

    SetParam("port", static_cast<int32_t>(6454));
    SetParam("universe", static_cast<int32_t>(0));
    SetParam("num_channels", static_cast<int32_t>(512));
    SetParam("normalized", true);
    SetParam("active", true);
}

DMXInChanOp::~DMXInChanOp() = default;

bool DMXInChanOp::Cook(const CookContext& /*context*/) {
    bool active = true;
    if (GetParam("active").Is<bool>()) {
        active = GetParam("active").Get<bool>();
    }

    int32_t port = 6454;
    if (GetParam("port").Is<int32_t>()) {
        port = GetParam("port").Get<int32_t>();
    }

    int32_t universe = 0;
    if (GetParam("universe").Is<int32_t>()) {
        universe = std::clamp(GetParam("universe").Get<int32_t>(), 0, 32767);
    }

    int32_t numChans = 512;
    if (GetParam("num_channels").Is<int32_t>()) {
        numChans = std::clamp(GetParam("num_channels").Get<int32_t>(), 1, 512);
    }

    bool normalized = true;
    if (GetParam("normalized").Is<bool>()) {
        normalized = GetParam("normalized").Get<bool>();
    }

    auto& engine = ArtNetEngine::Instance();
    if (active) {
        if (!engine.IsReceiverRunning()) {
            engine.StartReceiver(static_cast<uint16_t>(port));
        }
    }

    const auto& allNames = GetDMXChannelNames();
    std::vector<std::string> names(allNames.begin(), allNames.begin() + numChans);

    ChannelBuffer buf(names, 1, 60.0f);
    if (active && numChans > 0) {
        float arr[512] = {0};
        engine.GetUniverseChannels(static_cast<uint16_t>(universe), arr, static_cast<size_t>(numChans), normalized);
        for (int i = 0; i < numChans; ++i) {
            float* p = buf.GetChannelData(static_cast<size_t>(i));
            if (p) *p = arr[i];
        }
    }

    SetOutputBuffer(buf);
    return true;
}

} // namespace nf
