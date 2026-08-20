#include <gtest/gtest.h>
#include "../../sdk/include/nf_plugin_abi.h"
#include "../../sdk/include/NodeForgePluginSDK.hpp"

// ─── Test Dummy Plugins for ABI verification ─────────────────────────────────
class TestThrowingPlugin : public nf::sdk::PluginNode {
public:
    using PluginNode::PluginNode;
    NF_Result Cook(const NF_CookContext& /*ctx*/) override {
        throw std::runtime_error("Simulated plugin crash!");
    }
};

class TestMathChanPlugin : public nf::sdk::ChanOpPlugin {
public:
    using ChanOpPlugin::ChanOpPlugin;

    NF_Result Cook(const NF_CookContext& /*ctx*/) override {
        float scale = GetFloat("scale", 2.0f);
        m_data[0] = 10.0f * scale;
        m_data[1] = 20.0f * scale;

        m_ptrs[0] = m_data;
        m_desc.channelCount = 1;
        m_desc.sampleCount = 2;
        m_desc.sampleRate = 60.0;
        m_desc.channelNames = nullptr;
        m_desc.channelData = m_ptrs;

        if (m_outputChannels.empty()) m_outputChannels.resize(1);
        m_outputChannels[0] = m_desc;
        return NF_SUCCESS;
    }

private:
    float m_data[2] = { 0.0f, 0.0f };
    float* m_ptrs[1] = { nullptr };
    NF_ChannelBufferDescriptor m_desc{};
};

// ─── Test Cases ──────────────────────────────────────────────────────────────
TEST(PluginABITest, VersionHandshake) {
    EXPECT_EQ(NF_PLUGIN_ABI_VERSION_MAJOR, 1);
    EXPECT_EQ(NF_PLUGIN_ABI_VERSION_MINOR, 0);
    uint32_t expectedVer = (1 << 16) | (0 << 8) | 0;
    EXPECT_EQ(NF_PLUGIN_ABI_VERSION, expectedVer);
}

TEST(PluginABITest, StructAlignment) {
    EXPECT_GT(sizeof(NF_PluginInfo), 0u);
    EXPECT_GT(sizeof(NF_PinDef), 0u);
    EXPECT_GT(sizeof(NF_ParamDef), 0u);
    EXPECT_GT(sizeof(NF_NodeVTable), 0u);
    EXPECT_GT(sizeof(NF_CookContext), 0u);
    EXPECT_GT(sizeof(NF_ChannelBufferDescriptor), 0u);
    EXPECT_GT(sizeof(NF_DataTableDescriptor), 0u);
    EXPECT_GT(sizeof(NF_VulkanTextureDescriptor), 0u);
}

TEST(PluginABITest, VTableBinderExceptionSafety) {
    NF_NodeVTable vt = nf::sdk::VTableBinder<TestThrowingPlugin>::MakeVTable();
    ASSERT_NE(vt.createInstance, nullptr);
    ASSERT_NE(vt.destroyInstance, nullptr);
    ASSERT_NE(vt.cook, nullptr);

    NF_PluginInstanceHandle inst = vt.createInstance(101, "ThrowingNode");
    ASSERT_NE(inst, nullptr);

    NF_CookContext ctx{};
    ctx.frameNumber = 1;
    ctx.timeSeconds = 0.016;

    // Must catch exception internally and return NF_ERROR_EXECUTION_FAILED without crashing
    NF_Result res = vt.cook(inst, &ctx);
    EXPECT_EQ(res, NF_ERROR_EXECUTION_FAILED);

    vt.destroyInstance(inst);
}

TEST(PluginABITest, VTableBinderParameterAndCook) {
    NF_NodeVTable vt = nf::sdk::VTableBinder<TestMathChanPlugin>::MakeVTable();
    NF_PluginInstanceHandle inst = vt.createInstance(202, "MathChanNode");
    ASSERT_NE(inst, nullptr);

    // Set parameter
    vt.setParamFloat(inst, "scale", 3.0f);

    NF_CookContext ctx{};
    ctx.frameNumber = 1;
    NF_Result res = vt.cook(inst, &ctx);
    EXPECT_EQ(res, NF_SUCCESS);

    NF_ChannelBufferDescriptor outDesc{};
    NF_Result getRes = vt.getOutputChannels(inst, 0, &outDesc);
    EXPECT_EQ(getRes, NF_SUCCESS);
    EXPECT_EQ(outDesc.channelCount, 1u);
    EXPECT_EQ(outDesc.sampleCount, 2u);
    ASSERT_NE(outDesc.channelData, nullptr);
    EXPECT_FLOAT_EQ(outDesc.channelData[0][0], 30.0f);
    EXPECT_FLOAT_EQ(outDesc.channelData[0][1], 60.0f);

    vt.destroyInstance(inst);
}
