#include <gtest/gtest.h>
#include "../../src/plugin/PluginManager.h"
#include "../../src/plugin/PluginNodeProxy.h"
#include "../../src/graph/Graph.h"
#include "../../src/graph/NodeRegistry.h"
#include "../../sdk/include/NodeForgePluginSDK.hpp"

// ─── Mock In-Memory Plugin for Manager Testing ──────────────────────────────
class MockLFOPlugin : public nf::sdk::ChanOpPlugin {
public:
    using ChanOpPlugin::ChanOpPlugin;

    NF_Result Cook(const NF_CookContext& /*ctx*/) override {
        float val = GetFloat("val", 42.0f);
        m_samples[0] = val;
        m_ptrs[0] = m_samples;

        static const char* s_names[] = { "out" };
        m_desc.channelCount = 1;
        m_desc.sampleCount = 1;
        m_desc.sampleRate = 60.0;
        m_desc.channelNames = s_names;
        m_desc.channelData = m_ptrs;

        if (m_outputChannels.empty()) m_outputChannels.resize(1);
        m_outputChannels[0] = m_desc;
        return NF_SUCCESS;
    }

private:
    float m_samples[1] = { 0.0f };
    float* m_ptrs[1] = { nullptr };
    NF_ChannelBufferDescriptor m_desc{};
};

TEST(PluginManagerTest, SearchPathsManagement) {
    auto& pm = nf::PluginManager::Instance();
    size_t initialCount = pm.GetSearchPaths().size();

    std::filesystem::path customPath = std::filesystem::current_path() / "test_plugins_dir";
    pm.AddSearchPath(customPath);

    auto paths = pm.GetSearchPaths();
    EXPECT_GE(paths.size(), initialCount);

    // Duplicate additions should be ignored
    pm.AddSearchPath(customPath);
    EXPECT_EQ(pm.GetSearchPaths().size(), paths.size());
}

TEST(PluginManagerTest, ProxyNodeInstantiationAndCook) {
    nf::PluginOperatorInfo opInfo;
    opInfo.typeName = "Chan.MockLFO";
    opInfo.family = NF_NODE_FAMILY_CHANOP;
    opInfo.category = "Test";
    opInfo.description = "Mock LFO Plugin Operator";

    NF_PinDef pin{};
    pin.name = "Output";
    pin.direction = NF_PIN_DIR_OUTPUT;
    pin.type = NF_PIN_TYPE_CHANNEL;
    opInfo.pinDefs.push_back(pin);

    NF_ParamDef param{};
    param.name = "val";
    param.label = "Value";
    param.type = NF_PARAM_FLOAT;
    param.defaultFloat = 99.0f;
    opInfo.paramDefs.push_back(param);

    opInfo.vtable = nf::sdk::VTableBinder<MockLFOPlugin>::MakeVTable();

    // Register into NodeRegistry
    nf::NodeTypeInfo typeInfo;
    typeInfo.typeName = opInfo.typeName;
    typeInfo.family = nf::NodeFamily::ChanOp;
    typeInfo.category = opInfo.category;
    typeInfo.description = opInfo.description;
    typeInfo.factory = [opInfo](nf::NodeId id, const std::string& name) -> std::unique_ptr<nf::Node> {
        return std::make_unique<nf::PluginNodeProxy>(id, name, opInfo);
    };

    nf::NodeRegistry::Instance().Register(typeInfo);

    // Instantiate inside a Graph
    nf::Graph graph;
    auto node = nf::NodeRegistry::Instance().CreateNode("Chan.MockLFO", graph.GenerateNodeId(), "MockLFO1");
    ASSERT_NE(node, nullptr);
    nf::NodeId nId = node->GetId();
    graph.AddNode(std::move(node));

    nf::Node* liveNode = graph.GetNode(nId);
    ASSERT_NE(liveNode, nullptr);
    EXPECT_EQ(liveNode->GetOutputPins().size(), 1u);
    EXPECT_TRUE(liveNode->HasParam("val"));

    // Set parameter
    liveNode->SetParam("val", nf::PinValue(123.45f));

    // Cook
    nf::CookContext ctx{};
    ctx.frameIndex = 1;
    ctx.timeSeconds = 0.016;
    bool cookSuccess = graph.CookNode(liveNode, ctx);
    EXPECT_TRUE(cookSuccess);

    const auto& outVal = liveNode->GetOutputPin(0)->GetValue();
    EXPECT_TRUE(outVal.Is<nf::ChannelBuffer>());
    EXPECT_FLOAT_EQ(outVal.Get<nf::ChannelBuffer>().GetChannelData(0)[0], 123.45f);
}
