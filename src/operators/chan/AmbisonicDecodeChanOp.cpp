#include "AmbisonicDecodeChanOp.h"
#include <cmath>

namespace nf {

AmbisonicDecodeChanOp::AmbisonicDecodeChanOp(NodeId id, const std::string& name)
    : Node(id, name, "AmbisonicDecodeChanOp") {
    m_inBFormatPin = AddInputPin("in_bformat", PinType::Chan);
    m_inLayoutDataPin = AddInputPin("speaker_layout_data", PinType::Data);
    m_outSpeakersPin = AddOutputPin("out_speakers", PinType::Chan);

    SetParam("target_layout", static_cast<int32_t>(2)); // 0: Stereo, 1: Quad, 2: 5.1, 3: 7.1.4, 4: Custom
    SetParam("shelf_filter_hf", true);

    m_outBuffer.Resize(2, 1);
    m_outBuffer.SetChannelNames({"ch_L", "ch_R"});
}

bool AmbisonicDecodeChanOp::Cook(const CookContext& /*context*/) {
    if (!m_inBFormatPin || !m_inBFormatPin->GetValue().Is<ChannelBuffer>()) {
        return false;
    }

    const auto& bformat = m_inBFormatPin->GetValue().Get<ChannelBuffer>();
    if (bformat.GetChannelCount() < 4 || bformat.GetSampleCount() == 0) {
        return false;
    }

    size_t sampleCount = bformat.GetSampleCount();
    const float* w = bformat.GetChannelData(0);
    const float* x = bformat.GetChannelData(1);
    const float* y = bformat.GetChannelData(2);
    const float* z = bformat.GetChannelData(3);

    int32_t targetLayout = GetParam("target_layout").Is<int32_t>() ? GetParam("target_layout").Get<int32_t>() : 2;

    if (targetLayout == 0) {
        // Stereo (Left: theta = -90, Right: theta = +90)
        m_outBuffer.Resize(2, sampleCount);
        m_outBuffer.SetChannelNames({"ch_L", "ch_R"});

        float* l = m_outBuffer.GetChannelData(0);
        float* r = m_outBuffer.GetChannelData(1);

        for (size_t s = 0; s < sampleCount; ++s) {
            float baseW = w[s] * 0.70710678f;
            l[s] = baseW - y[s] * 0.5f;
            r[s] = baseW + y[s] * 0.5f;
        }
    } else if (targetLayout == 1) {
        // Quadraphonic 4.0
        m_outBuffer.Resize(4, sampleCount);
        m_outBuffer.SetChannelNames({"ch_FL", "ch_FR", "ch_RL", "ch_RR"});

        float* fl = m_outBuffer.GetChannelData(0);
        float* fr = m_outBuffer.GetChannelData(1);
        float* rl = m_outBuffer.GetChannelData(2);
        float* rr = m_outBuffer.GetChannelData(3);

        for (size_t s = 0; s < sampleCount; ++s) {
            float baseW = w[s] * 0.5f;
            fl[s] = baseW + 0.3535f * (x[s] - y[s]);
            fr[s] = baseW + 0.3535f * (x[s] + y[s]);
            rl[s] = baseW + 0.3535f * (-x[s] - y[s]);
            rr[s] = baseW + 0.3535f * (-x[s] + y[s]);
        }
    } else if (targetLayout == 2) {
        // 5.1 Surround
        m_outBuffer.Resize(6, sampleCount);
        m_outBuffer.SetChannelNames({"ch_L", "ch_R", "ch_C", "ch_LFE", "ch_Ls", "ch_Rs"});

        float* l = m_outBuffer.GetChannelData(0);
        float* r = m_outBuffer.GetChannelData(1);
        float* c = m_outBuffer.GetChannelData(2);
        float* lfe = m_outBuffer.GetChannelData(3);
        float* ls = m_outBuffer.GetChannelData(4);
        float* rs = m_outBuffer.GetChannelData(5);

        for (size_t s = 0; s < sampleCount; ++s) {
            float baseW = w[s] * 0.35f;
            l[s] = baseW + 0.3f * x[s] - 0.3f * y[s];
            r[s] = baseW + 0.3f * x[s] + 0.3f * y[s];
            c[s] = baseW + 0.45f * x[s];
            lfe[s] = w[s] * 0.2f;
            ls[s] = baseW - 0.25f * x[s] - 0.35f * y[s];
            rs[s] = baseW - 0.25f * x[s] + 0.35f * y[s];
        }
    } else {
        // 7.1.4 Dolby Atmos Dome (12 channels)
        m_outBuffer.Resize(12, sampleCount);
        m_outBuffer.SetChannelNames({
            "ch_L", "ch_R", "ch_C", "ch_LFE",
            "ch_Ls", "ch_Rs", "ch_Rls", "ch_Rrs",
            "ch_Tfl", "ch_Tfr", "ch_Trl", "ch_Trr"
        });

        for (size_t ch = 0; ch < 12; ++ch) {
            float* d = m_outBuffer.GetChannelData(ch);
            float angle = static_cast<float>(ch % 8) * (6.2831853f / 8.0f) - 3.14159265f;
            float height = (ch >= 8) ? 0.4f : 0.0f;

            for (size_t s = 0; s < sampleCount; ++s) {
                d[s] = (w[s] * 0.2f) + 0.25f * (x[s] * std::cos(angle) + y[s] * std::sin(angle)) + z[s] * height;
            }
        }
    }

    m_outSpeakersPin->SetValue(PinValue(m_outBuffer));
    return true;
}

} // namespace nf
