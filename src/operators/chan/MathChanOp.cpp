#include "MathChanOp.h"
#include <cmath>
#include <algorithm>

namespace nf {

MathChanOp::MathChanOp(NodeId id, const std::string& name)
    : ChanOp(id, name, "MathChanOp") {
    m_inPin1 = AddInputPin("input", PinType::Chan);
    m_inPin2 = AddInputPin("input2", PinType::Chan);
    m_outPin = AddOutputPin("output", PinType::Chan);

    SetParam("operation", static_cast<int32_t>(0)); // 0: Add, 1: Multiply, 2: Sin, 3: Cos, 4: Abs, 5: Clamp, 6: Combine Inputs
    SetParam("combine_mode", static_cast<int32_t>(0)); // 0: Add, 1: Subtract, 2: Multiply, 3: Divide, 4: Min, 5: Max, 6: Average
    SetParam("scalar", 0.0f);
    SetParam("range_in_min", 0.0f);
    SetParam("range_in_max", 1.0f);
    SetParam("range_out_min", 0.0f);
    SetParam("range_out_max", 1.0f);
}

bool MathChanOp::Cook(const CookContext& /*context*/) {
    const ChannelBuffer* inBuf1 = GetInputBuffer(0);
    const ChannelBuffer* inBuf2 = GetInputBuffer(1);

    if (!inBuf1 || inBuf1->IsEmpty()) {
        SetOutputBuffer(ChannelBuffer{});
        return true;
    }

    int32_t op = GetParam("operation").Is<int32_t>() ? GetParam("operation").Get<int32_t>() : 0;
    int32_t combineMode = GetParam("combine_mode").Is<int32_t>() ? GetParam("combine_mode").Get<int32_t>() : 0;
    float scalar = GetParam("scalar").Is<float>() ? GetParam("scalar").Get<float>() : 0.0f;

    float inMin = GetParam("range_in_min").Is<float>() ? GetParam("range_in_min").Get<float>() : 0.0f;
    float inMax = GetParam("range_in_max").Is<float>() ? GetParam("range_in_max").Get<float>() : 1.0f;
    float outMin = GetParam("range_out_min").Is<float>() ? GetParam("range_out_min").Get<float>() : 0.0f;
    float outMax = GetParam("range_out_max").Is<float>() ? GetParam("range_out_max").Get<float>() : 1.0f;

    bool doRemap = (inMin != outMin || inMax != outMax) && (std::abs(inMax - inMin) > 1e-6f);

    ChannelBuffer outBuf = *inBuf1;

    // Operation 6: Combine Two Inputs
    if (op == 6 && inBuf2 && !inBuf2->IsEmpty()) {
        size_t commonChans = std::min(outBuf.GetChannelCount(), inBuf2->GetChannelCount());
        size_t commonSamples = std::min(outBuf.GetSampleCount(), inBuf2->GetSampleCount());

        for (size_t c = 0; c < commonChans; ++c) {
            float* d1 = outBuf.GetChannelData(c);
            const float* d2 = inBuf2->GetChannelData(c);
            for (size_t s = 0; s < commonSamples; ++s) {
                switch (combineMode) {
                    case 0: d1[s] += d2[s]; break; // Add
                    case 1: d1[s] -= d2[s]; break; // Sub
                    case 2: d1[s] *= d2[s]; break; // Mult
                    case 3: d1[s] = (std::abs(d2[s]) > 1e-7f) ? (d1[s] / d2[s]) : 0.0f; break; // Div
                    case 4: d1[s] = std::min(d1[s], d2[s]); break; // Min
                    case 5: d1[s] = std::max(d1[s], d2[s]); break; // Max
                    case 6: d1[s] = (d1[s] + d2[s]) * 0.5f; break; // Average
                    default: d1[s] += d2[s]; break;
                }
            }
        }
    } else {
        // Unary / Scalar Math Operations
        switch (op) {
            case 0: // Add scalar
                outBuf.Add(scalar);
                break;
            case 1: // Multiply scalar
                outBuf.Multiply(scalar);
                break;
            case 2: // Sin
                outBuf.Sin();
                break;
            case 3: // Cos
                outBuf.Cos();
                break;
            case 4: // Abs
                outBuf.Abs();
                break;
            case 5: // Clamp
                outBuf.Clamp(inMin, inMax);
                break;
            default:
                break;
        }
    }

    if (doRemap) {
        outBuf.Remap(inMin, inMax, outMin, outMax);
    }

    SetOutputBuffer(outBuf);
    return true;
}

} // namespace nf
