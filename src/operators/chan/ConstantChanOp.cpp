#include "ConstantChanOp.h"
#include <sstream>

namespace nf {

ConstantChanOp::ConstantChanOp(NodeId id, const std::string& name)
    : Node(id, name, "ConstantChanOp") {
    m_outPin = AddOutputPin("output", PinType::Chan);

    SetParam("channel_names", std::string("chan1 chan2"));
    SetParam("values", glm::vec4(1.0f, 2.0f, 0.0f, 0.0f));
    SetParam("sample_rate", 60.0f);
    SetParam("sample_count", static_cast<int32_t>(1));
}

bool ConstantChanOp::Cook(const CookContext& /*context*/) {
    std::string chanNamesStr = "chan1";
    if (GetParam("channel_names").Is<std::string>()) {
        chanNamesStr = GetParam("channel_names").Get<std::string>();
    }

    glm::vec4 vals(1.0f, 0.0f, 0.0f, 0.0f);
    if (GetParam("values").Is<glm::vec4>()) {
        vals = GetParam("values").Get<glm::vec4>();
    } else if (GetParam("values").Is<float>()) {
        vals.x = GetParam("values").Get<float>();
    }

    float rate = 60.0f;
    if (GetParam("sample_rate").Is<float>()) {
        rate = GetParam("sample_rate").Get<float>();
    }

    int32_t numSamples = 1;
    if (GetParam("sample_count").Is<int32_t>()) {
        numSamples = std::max(1, GetParam("sample_count").Get<int32_t>());
    }

    // Split channel names
    std::vector<std::string> names;
    std::stringstream ss(chanNamesStr);
    std::string item;
    while (ss >> item) {
        names.push_back(item);
    }
    if (names.empty()) {
        names.push_back("chan1");
    }

    ChannelBuffer buf;
    buf.sampleRate = rate;

    const float valArray[4] = { vals.x, vals.y, vals.z, vals.w };

    for (size_t i = 0; i < names.size(); ++i) {
        float sampleVal = (i < 4) ? valArray[i] : 0.0f;
        std::vector<float> samples(numSamples, sampleVal);
        buf.AddChannel(names[i], samples);
    }

    m_outPin->SetValue(PinValue(buf));
    return true;
}

} // namespace nf
