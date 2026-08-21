#include "DialPanelComp.h"
#include <algorithm>
#include <cmath>

namespace nf {

DialPanelComp::DialPanelComp(NodeId id, const std::string& name)
    : Node(id, name, "DialPanelComp") {
    m_inChanPin = AddInputPin("in_chan", PinType::Chan);
    m_outCompPin = AddOutputPin("output", PinType::Any);
    m_outChanPin = AddOutputPin("out_chan", PinType::Chan);

    SetParam("label", std::string("Knob"));
    SetParam("min_value", 0.0f);
    SetParam("max_value", 100.0f);
    SetParam("value", 50.0f);
    SetParam("bipolar", false);
    SetParam("sweep_angle", 270.0f);

    m_outBuffer.Resize(3, 1);
    m_outBuffer.SetChannelNames({"val", "norm_val", "angle"});
    m_outBuffer.GetChannelData(0)[0] = 50.0f;
    m_outBuffer.GetChannelData(1)[0] = 0.5f;
    m_outBuffer.GetChannelData(2)[0] = 0.0f;
}

float DialPanelComp::GetValue() const {
    return GetParam("value").Is<float>() ? GetParam("value").Get<float>() : 50.0f;
}

float DialPanelComp::GetAngleDegrees() const {
    float val = GetValue();
    float minV = GetParam("min_value").Is<float>() ? GetParam("min_value").Get<float>() : 0.0f;
    float maxV = GetParam("max_value").Is<float>() ? GetParam("max_value").Get<float>() : 100.0f;
    float sweep = GetParam("sweep_angle").Is<float>() ? GetParam("sweep_angle").Get<float>() : 270.0f;
    float range = maxV - minV;
    float norm = std::abs(range) > 1e-5f ? std::clamp((val - minV) / range, 0.0f, 1.0f) : 0.0f;
    return -sweep * 0.5f + norm * sweep;
}

void DialPanelComp::SetValueInteractive(float val) {
    float minV = GetParam("min_value").Is<float>() ? GetParam("min_value").Get<float>() : 0.0f;
    float maxV = GetParam("max_value").Is<float>() ? GetParam("max_value").Get<float>() : 100.0f;
    val = std::clamp(val, std::min(minV, maxV), std::max(minV, maxV));
    SetParam("value", val);
}

bool DialPanelComp::Cook(const CookContext& /*context*/) {
    if (m_inChanPin && m_inChanPin->GetValue().Is<ChannelBuffer>()) {
        const auto& inBuf = m_inChanPin->GetValue().Get<ChannelBuffer>();
        if (inBuf.GetChannelCount() > 0 && inBuf.GetSampleCount() > 0) {
            SetValueInteractive(inBuf.GetChannelData(0)[0]);
        }
    }

    float val = GetValue();
    float minV = GetParam("min_value").Is<float>() ? GetParam("min_value").Get<float>() : 0.0f;
    float maxV = GetParam("max_value").Is<float>() ? GetParam("max_value").Get<float>() : 100.0f;
    float range = maxV - minV;
    float normVal = std::abs(range) > 1e-5f ? std::clamp((val - minV) / range, 0.0f, 1.0f) : 0.0f;
    float angle = GetAngleDegrees();

    m_outBuffer.Resize(3, 1);
    m_outBuffer.SetChannelNames({"val", "norm_val", "angle"});
    m_outBuffer.GetChannelData(0)[0] = val;
    m_outBuffer.GetChannelData(1)[0] = normVal;
    m_outBuffer.GetChannelData(2)[0] = angle;

    m_outChanPin->SetValue(PinValue(m_outBuffer));
    return true;
}

} // namespace nf
