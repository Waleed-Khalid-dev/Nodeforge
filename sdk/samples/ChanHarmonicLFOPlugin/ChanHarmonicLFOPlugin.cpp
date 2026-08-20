#include "../../include/NodeForgePluginSDK.hpp"
#include <cmath>
#include <vector>

class ChanHarmonicLFOPlugin : public nf::sdk::ChanOpPlugin {
public:
    using ChanOpPlugin::ChanOpPlugin;

    NF_Result Cook(const NF_CookContext& ctx) override {
        float freq = GetFloat("frequency", 1.0f);
        float amp = GetFloat("amplitude", 1.0f);
        int32_t sampleCount = GetInt("sampleCount", 64);
        if (sampleCount <= 0) sampleCount = 64;

        m_buffer.resize(sampleCount);
        m_harmonics.resize(sampleCount);

        double dt = 1.0 / 60.0;
        double phase = ctx.timeSeconds * freq * 2.0 * 3.14159265358979323846;

        for (int32_t i = 0; i < sampleCount; ++i) {
            double t = phase + (i * dt * freq * 2.0 * 3.14159265358979323846);
            float f1 = static_cast<float>(std::sin(t));
            float f3 = static_cast<float>(std::sin(3.0 * t) / 3.0);
            float f5 = static_cast<float>(std::sin(5.0 * t) / 5.0);
            m_buffer[i] = (f1 + f3 + f5) * amp;
            m_harmonics[i] = f3 * amp;
        }

        m_channelPointers[0] = m_buffer.data();
        m_channelPointers[1] = m_harmonics.data();

        static const char* s_names[] = { "composite", "harmonic3" };

        NF_ChannelBufferDescriptor desc{};
        desc.channelCount = 2;
        desc.sampleCount = static_cast<uint32_t>(sampleCount);
        desc.sampleRate = 60.0;
        desc.channelNames = s_names;
        desc.channelData = m_channelPointers;

        if (m_outputChannels.empty()) {
            m_outputChannels.resize(1);
        }
        m_outputChannels[0] = desc;

        return NF_SUCCESS;
    }

private:
    std::vector<float> m_buffer;
    std::vector<float> m_harmonics;
    float* m_channelPointers[2] = { nullptr, nullptr };
};

static const NF_PinDef s_lfoPins[] = {
    { "Output", NF_PIN_DIR_OUTPUT, NF_PIN_TYPE_CHANNEL }
};

static const NF_ParamDef s_lfoParams[] = {
    { "frequency", "Frequency (Hz)", "Oscillator", NF_PARAM_FLOAT, 1.0f, 0, false, nullptr, 0.01f, 100.0f, 0.1f, nullptr, 0 },
    { "amplitude", "Amplitude", "Oscillator", NF_PARAM_FLOAT, 1.0f, 0, false, nullptr, 0.0f, 10.0f, 0.1f, nullptr, 0 },
    { "sampleCount", "Sample Count", "Buffer", NF_PARAM_INT, 64.0f, 64, false, nullptr, 1.0f, 1024.0f, 1.0f, nullptr, 0 }
};

static const NF_PluginInfo s_lfoPluginInfo = {
    "ChanHarmonicLFOPlugin",
    "Neo Realms",
    "Multi-Harmonic Fourier LFO Generator",
    "https://neorealms.io",
    1, 0, 0,
    NF_PLUGIN_ABI_VERSION,
    1
};

extern "C" {

NF_PLUGIN_API const NF_PluginInfo* NF_GetPluginInfo(void) {
    return &s_lfoPluginInfo;
}

NF_PLUGIN_API int32_t NF_RegisterOperators(NF_OperatorDef* outOps, uint32_t maxOps) {
    if (!outOps || maxOps < 1) return 0;

    outOps[0].typeName = "Chan.HarmonicLFOPlugin";
    outOps[0].family = NF_NODE_FAMILY_CHANOP;
    outOps[0].category = "Custom/Generator";
    outOps[0].description = "Harmonic Fourier Synthesis LFO Channel Operator";
    outOps[0].author = "Neo Realms";
    outOps[0].version = 1;
    outOps[0].pinDefs = s_lfoPins;
    outOps[0].pinDefCount = 1;
    outOps[0].paramDefs = s_lfoParams;
    outOps[0].paramDefCount = 3;
    outOps[0].vtable = nf::sdk::VTableBinder<ChanHarmonicLFOPlugin>::MakeVTable();

    return 1;
}

NF_PLUGIN_API void NF_UnloadPlugin(void) {}

}
