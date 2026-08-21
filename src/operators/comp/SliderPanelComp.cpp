#include "SliderPanelComp.h"
#include <algorithm>
#include <cmath>

namespace nf {

SliderPanelComp::SliderPanelComp(NodeId id, const std::string& name)
    : Node(id, name, "SliderPanelComp") {
    m_inChanPin = AddInputPin("in_chan", PinType::Chan);
    m_outCompPin = AddOutputPin("output", PinType::Any);
    m_outChanPin = AddOutputPin("out_chan", PinType::Chan);

    SetParam("label", std::string("Fader"));
    SetParam("orientation", static_cast<int32_t>(0)); // 0: Horizontal, 1: Vertical
    SetParam("min_value", 0.0f);
    SetParam("max_value", 1.0f);
    SetParam("value", 0.5f);
    SetParam("step", 0.0f);
    SetParam("accent_color", glm::vec4(0.0f, 0.8f, 1.0f, 1.0f));

    m_outBuffer.Resize(2, 1);
    m_outBuffer.SetChannelNames({"val", "norm_val"});
    m_outBuffer.GetChannelData(0)[0] = 0.5f;
    m_outBuffer.GetChannelData(1)[0] = 0.5f;
}

float SliderPanelComp::GetValue() const {
    return GetParam("value").Is<float>() ? GetParam("value").Get<float>() : 0.5f;
}

float SliderPanelComp::GetNormalizedValue() const {
    float val = GetValue();
    float minV = GetParam("min_value").Is<float>() ? GetParam("min_value").Get<float>() : 0.0f;
    float maxV = GetParam("max_value").Is<float>() ? GetParam("max_value").Get<float>() : 1.0f;
    float range = maxV - minV;
    return std::abs(range) > 1e-5f ? std::clamp((val - minV) / range, 0.0f, 1.0f) : 0.0f;
}

void SliderPanelComp::SetValueInteractive(float val) {
    float minV = GetParam("min_value").Is<float>() ? GetParam("min_value").Get<float>() : 0.0f;
    float maxV = GetParam("max_value").Is<float>() ? GetParam("max_value").Get<float>() : 1.0f;
    val = std::clamp(val, std::min(minV, maxV), std::max(minV, maxV));

    float step = GetParam("step").Is<float>() ? GetParam("step").Get<float>() : 0.0f;
    if (step > 0.0f) {
        val = minV + std::round((val - minV) / step) * step;
    }

    SetParam("value", val);
}

bool SliderPanelComp::Cook(const CookContext& /*context*/) {
    // If external hardware channel connected, read from it
    if (m_inChanPin && m_inChanPin->GetValue().Is<ChannelBuffer>()) {
        const auto& inBuf = m_inChanPin->GetValue().Get<ChannelBuffer>();
        if (inBuf.GetChannelCount() > 0 && inBuf.GetSampleCount() > 0) {
            SetValueInteractive(inBuf.GetChannelData(0)[0]);
        }
    }

    float val = GetValue();
    float normVal = GetNormalizedValue();

    m_outBuffer.Resize(2, 1);
    m_outBuffer.SetChannelNames({"val", "norm_val"});
    m_outBuffer.GetChannelData(0)[0] = val;
    m_outBuffer.GetChannelData(1)[0] = normVal;

    m_outChanPin->SetValue(PinValue(m_outBuffer));
    return true;
}

} // namespace nf
