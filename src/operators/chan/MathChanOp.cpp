#include "MathChanOp.h"
#include <cmath>
#include <algorithm>

namespace nf {

MathChanOp::MathChanOp(NodeId id, const std::string& name)
    : Node(id, name, "MathChanOp") {
    m_inPin = AddInputPin("input", PinType::Chan);
    m_outPin = AddOutputPin("output", PinType::Chan);

    SetParam("operation", static_cast<int32_t>(0)); // 0: Add, 1: Multiply, 2: Sin, 3: Cos, 4: Abs, 5: Clamp
    SetParam("scalar", 0.0f);
    SetParam("range_in_min", 0.0f);
    SetParam("range_in_max", 1.0f);
    SetParam("range_out_min", 0.0f);
    SetParam("range_out_max", 1.0f);
}

bool MathChanOp::Cook(const CookContext& /*context*/) {
    const PinValue& inVal = m_inPin->GetValue();
    if (!inVal.Is<ChannelBuffer>()) {
        m_outPin->SetValue(PinValue(ChannelBuffer{}));
        return true;
    }

    const ChannelBuffer& inBuf = inVal.Get<ChannelBuffer>();
    ChannelBuffer outBuf;
    outBuf.names = inBuf.names;
    outBuf.sampleRate = inBuf.sampleRate;
    outBuf.data.resize(inBuf.data.size());

    int32_t op = 0;
    if (GetParam("operation").Is<int32_t>()) {
        op = GetParam("operation").Get<int32_t>();
    }

    float scalar = 0.0f;
    if (GetParam("scalar").Is<float>()) {
        scalar = GetParam("scalar").Get<float>();
    }

    float inMin = 0.0f;
    if (GetParam("range_in_min").Is<float>()) inMin = GetParam("range_in_min").Get<float>();
    float inMax = 1.0f;
    if (GetParam("range_in_max").Is<float>()) inMax = GetParam("range_in_max").Get<float>();
    float outMin = 0.0f;
    if (GetParam("range_out_min").Is<float>()) outMin = GetParam("range_out_min").Get<float>();
    float outMax = 1.0f;
    if (GetParam("range_out_max").Is<float>()) outMax = GetParam("range_out_max").Get<float>();

    bool doRemap = (inMin != outMin || inMax != outMax) && (std::abs(inMax - inMin) > 1e-6f);

    for (size_t c = 0; c < inBuf.data.size(); ++c) {
        outBuf.data[c].resize(inBuf.data[c].size());
        for (size_t s = 0; s < inBuf.data[c].size(); ++s) {
            float v = inBuf.data[c][s];

            switch (op) {
                case 0: // Add
                    v += scalar;
                    break;
                case 1: // Multiply
                    v *= scalar;
                    break;
                case 2: // Sin
                    v = std::sin(v);
                    break;
                case 3: // Cos
                    v = std::cos(v);
                    break;
                case 4: // Abs
                    v = std::abs(v);
                    break;
                case 5: // Clamp
                    v = std::clamp(v, inMin, inMax);
                    break;
                default:
                    break;
            }

            if (doRemap) {
                float norm = (v - inMin) / (inMax - inMin);
                v = outMin + norm * (outMax - outMin);
            }

            outBuf.data[c][s] = v;
        }
    }

    m_outPin->SetValue(PinValue(outBuf));
    return true;
}

} // namespace nf
