#include "../../include/NodeForgePluginSDK.hpp"

class TexInvertPlugin : public nf::sdk::TexOpPlugin {
public:
    using TexOpPlugin::TexOpPlugin;

    NF_Result Cook(const NF_CookContext& /*ctx*/) override {
        // Invert shader pass / texture logic
        float amount = GetFloat("amount", 1.0f);
        bool invertAlpha = GetBool("invertAlpha", false);

        if (!m_inputTex.empty()) {
            m_outputTex = m_inputTex;
        }

        (void)amount;
        (void)invertAlpha;
        return NF_SUCCESS;
    }
};

static const NF_PinDef s_invertPins[] = {
    { "Input", NF_PIN_DIR_INPUT, NF_PIN_TYPE_TEXTURE2D },
    { "Output", NF_PIN_DIR_OUTPUT, NF_PIN_TYPE_TEXTURE2D }
};

static const NF_ParamDef s_invertParams[] = {
    { "amount", "Invert Amount", "Image", NF_PARAM_FLOAT, 1.0f, 0, false, nullptr, 0.0f, 1.0f, 0.01f, nullptr, 0 },
    { "invertAlpha", "Invert Alpha", "Image", NF_PARAM_BOOL, 0.0f, 0, false, nullptr, 0.0f, 1.0f, 1.0f, nullptr, 0 }
};

static const NF_PluginInfo s_texPluginInfo = {
    "TexInvertPlugin",
    "Neo Realms",
    "GPU Texture Color Inversion Plugin",
    "https://neorealms.io",
    1, 0, 0,
    NF_PLUGIN_ABI_VERSION,
    1
};

extern "C" {

NF_PLUGIN_API const NF_PluginInfo* NF_GetPluginInfo(void) {
    return &s_texPluginInfo;
}

NF_PLUGIN_API int32_t NF_RegisterOperators(NF_OperatorDef* outOps, uint32_t maxOps) {
    if (!outOps || maxOps < 1) return 0;

    outOps[0].typeName = "Tex.InvertPlugin";
    outOps[0].family = NF_NODE_FAMILY_TEXOP;
    outOps[0].category = "Custom/Filter";
    outOps[0].description = "Color and Alpha Inversion GPU Texture Operator";
    outOps[0].author = "Neo Realms";
    outOps[0].version = 1;
    outOps[0].pinDefs = s_invertPins;
    outOps[0].pinDefCount = 2;
    outOps[0].paramDefs = s_invertParams;
    outOps[0].paramDefCount = 2;
    outOps[0].vtable = nf::sdk::VTableBinder<TexInvertPlugin>::MakeVTable();

    return 1;
}

NF_PLUGIN_API void NF_UnloadPlugin(void) {}

}
