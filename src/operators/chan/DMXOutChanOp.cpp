#include "DMXOutChanOp.h"
#include "io/dmx/ArtNetEngine.h"
#include <algorithm>

namespace nf {

DMXOutChanOp::DMXOutChanOp(NodeId id, const std::string& name)
    : ChanOp(id, name, "DMXOutChanOp") {
    AddInputPin("input", PinType::Chan);
    m_outPin = AddOutputPin("output", PinType::Chan);

    SetParam("host", std::string("255.255.255.255"));
    SetParam("port", static_cast<int32_t>(6454));
    SetParam("universe", static_cast<int32_t>(0));
    SetParam("input_normalized", true);
    SetParam("active", true);
}

DMXOutChanOp::~DMXOutChanOp() = default;

bool DMXOutChanOp::Cook(const CookContext& /*context*/) {
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

    std::string host = "255.255.255.255";
    if (GetParam("host").Is<std::string>()) {
        host = GetParam("host").Get<std::string>();
    }

    int32_t port = 6454;
    if (GetParam("port").Is<int32_t>()) {
        port = GetParam("port").Get<int32_t>();
    }

    int32_t universe = 0;
    if (GetParam("universe").Is<int32_t>()) {
        universe = std::clamp(GetParam("universe").Get<int32_t>(), 0, 32767);
    }

    bool inputNormalized = true;
    if (GetParam("input_normalized").Is<bool>()) {
        inputNormalized = GetParam("input_normalized").Get<bool>();
    }

    std::vector<float> dmxData(512, 0.0f);
    size_t count = std::min(inBuf.GetChannelCount(), size_t(512));
    for (size_t i = 0; i < count; ++i) {
        const float* p = inBuf.GetChannelData(i);
        if (p) {
            float val = p[inBuf.GetSampleCount() - 1];
            dmxData[i] = inputNormalized ? val : (val / 255.0f);
        }
    }

    ArtNetEngine::Instance().SendDMX(host, static_cast<uint16_t>(port), static_cast<uint16_t>(universe), dmxData.data(), 512);

    return true;
}

} // namespace nf
