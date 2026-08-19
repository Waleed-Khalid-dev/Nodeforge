#include <gtest/gtest.h>
#include "operators/comp/ContainerComp.h"
#include "operators/comp/InOp.h"
#include "operators/comp/OutOp.h"
#include "operators/chan/ConstantChanOp.h"
#include "operators/chan/MathChanOp.h"
#include "project/ComponentSerializer.h"
#include "graph/CoreNodes.h"
#include <filesystem>

using namespace nf;
using namespace nf::project;

class ComponentHierarchyTest : public ::testing::Test {
protected:
    void SetUp() override {
        RegisterCoreNodes(NodeRegistry::Instance());
        m_tempDir = std::filesystem::temp_directory_path() / "nodeforge_comp_test";
        std::filesystem::create_directories(m_tempDir);
    }

    void TearDown() override {
        std::error_code ec;
        std::filesystem::remove_all(m_tempDir, ec);
    }

    std::filesystem::path m_tempDir;
};

TEST_F(ComponentHierarchyTest, DynamicBoundaryPinSync) {
    auto comp = std::make_unique<ContainerComp>(1, "Container1");
    Graph* inner = comp->GetInnerGraph();
    ASSERT_NE(inner, nullptr);

    // Initially no boundary operators
    EXPECT_EQ(comp->GetInputPins().size(), 0u);
    EXPECT_EQ(comp->GetOutputPins().size(), 0u);

    // Add InOp
    auto inOp = std::make_unique<InOp>(10, "In1");
    inOp->SetParam("pin_name", std::string("video_in"));
    inner->AddNode(std::move(inOp));

    // Add OutOp
    auto outOp = std::make_unique<OutOp>(20, "Out1");
    outOp->SetParam("pin_name", std::string("video_out"));
    inner->AddNode(std::move(outOp));

    // Sync
    comp->SynchronizeBoundaryPins();

    EXPECT_NE(comp->GetInputPin("video_in"), nullptr);
    EXPECT_NE(comp->GetOutputPin("video_out"), nullptr);
}

TEST_F(ComponentHierarchyTest, ContainerSubnetworkCook) {
    Graph rootGraph;

    // 1. Root Source Node
    auto constNode = std::make_unique<ConstantChanOp>(1, "Const1");
    constNode->SetParam("values", glm::vec4(5.0f, 10.0f, 0.0f, 0.0f));
    constNode->SetParam("channel_names", std::string("chan1 chan2"));
    rootGraph.AddNode(std::move(constNode));

    // 2. Container Node
    auto container = std::make_unique<ContainerComp>(2, "Subnetwork1");
    Graph* inner = container->GetInnerGraph();

    auto inOp = std::make_unique<InOp>(10, "In1");
    inOp->SetParam("pin_name", std::string("input_channels"));
    inner->AddNode(std::move(inOp));

    auto mathOp = std::make_unique<MathChanOp>(11, "MathMult");
    mathOp->SetParam("operation", static_cast<int32_t>(1)); // Multiply
    mathOp->SetParam("scalar", 3.0f);
    inner->AddNode(std::move(mathOp));

    auto outOp = std::make_unique<OutOp>(12, "Out1");
    outOp->SetParam("pin_name", std::string("output_channels"));
    inner->AddNode(std::move(outOp));

    // Connect inside container: In1 -> MathMult -> Out1
    inner->Connect(inner->GetNode(10)->GetOutputPin("output"), inner->GetNode(11)->GetInputPin("input"));
    inner->Connect(inner->GetNode(11)->GetOutputPin("output"), inner->GetNode(12)->GetInputPin("input"));

    container->SynchronizeBoundaryPins();
    ContainerComp* compPtr = container.get();
    rootGraph.AddNode(std::move(container));

    // Connect in root: Const1 -> Subnetwork1
    rootGraph.Connect(rootGraph.GetNode(1)->GetOutputPin("output"), compPtr->GetInputPin("input_channels"));

    // Cook root graph
    CookContext ctx{};
    EXPECT_TRUE(rootGraph.CookAll(ctx));

    // Check boundary output
    Pin* finalOut = compPtr->GetOutputPin("output_channels");
    ASSERT_NE(finalOut, nullptr);
    const PinValue& resultVal = finalOut->GetValue();
    ASSERT_TRUE(resultVal.Is<ChannelBuffer>());
    const auto& buffer = resultVal.Get<ChannelBuffer>();
    ASSERT_EQ(buffer.GetChannelCount(), 2u);
    EXPECT_FLOAT_EQ(buffer.data[0][0], 15.0f); // 5.0 * 3
    EXPECT_FLOAT_EQ(buffer.data[1][0], 30.0f); // 10.0 * 3
}

TEST_F(ComponentHierarchyTest, ComponentExportAndImport) {
    auto comp = std::make_unique<ContainerComp>(1, "BlurSubnet");
    Graph* inner = comp->GetInnerGraph();

    auto inOp = std::make_unique<InOp>(10, "In1");
    inner->AddNode(std::move(inOp));

    auto outOp = std::make_unique<OutOp>(20, "Out1");
    inner->AddNode(std::move(outOp));

    comp->SynchronizeBoundaryPins();

    // Export to .nfc
    std::string compFile = (m_tempDir / "BlurSubnet.nfc").string();
    std::string expErr;
    EXPECT_TRUE(ComponentSerializer::ExportComponent(compFile, comp.get(), &expErr)) << expErr;
    EXPECT_TRUE(std::filesystem::exists(compFile));

    // Import into fresh target graph
    Graph targetGraph;
    std::string impErr;
    ContainerComp* importedComp = ComponentSerializer::ImportComponent(compFile, &targetGraph, glm::vec2(100.0f, 100.0f), &impErr);
    ASSERT_NE(importedComp, nullptr) << impErr;

    EXPECT_EQ(importedComp->GetName(), "BlurSubnet");
    ASSERT_NE(importedComp->GetInnerGraph(), nullptr);
    EXPECT_EQ(importedComp->GetInnerGraph()->GetNodes().size(), 2u);
}
