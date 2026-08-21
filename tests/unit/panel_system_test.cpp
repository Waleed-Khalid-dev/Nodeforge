#include <gtest/gtest.h>
#include "operators/comp/PanelComp.h"
#include "operators/comp/SliderPanelComp.h"
#include "operators/comp/ButtonPanelComp.h"
#include "operators/comp/DialPanelComp.h"
#include "operators/comp/TextEntryPanelComp.h"
#include "project/ProjectSerializer.h"
#include "graph/CoreNodes.h"
#include <filesystem>

using namespace nf;
using namespace nf::project;

class PanelSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        RegisterCoreNodes(NodeRegistry::Instance());
    }
};

TEST_F(PanelSystemTest, SliderPanelCompInteractive) {
    auto slider = std::make_unique<SliderPanelComp>(1, "TestSlider");
    slider->SetParam("min_value", 10.0f);
    slider->SetParam("max_value", 50.0f);
    slider->SetParam("step", 5.0f);

    slider->SetValueInteractive(23.2f);
    // Snaps to nearest 5.0 -> 25.0
    EXPECT_FLOAT_EQ(slider->GetValue(), 25.0f);

    CookContext ctx{};
    EXPECT_TRUE(slider->Cook(ctx));

    const auto& chanVal = slider->GetOutputPin("out_chan")->GetValue();
    ASSERT_TRUE(chanVal.Is<ChannelBuffer>());
    const auto& buf = chanVal.Get<ChannelBuffer>();
    EXPECT_EQ(buf.GetChannelCount(), 2u);
    EXPECT_FLOAT_EQ(buf.GetChannelData(0)[0], 25.0f); // val
    EXPECT_FLOAT_EQ(buf.GetChannelData(1)[0], 0.375f); // (25-10)/40 = 15/40 = 0.375
}

TEST_F(PanelSystemTest, ButtonPanelCompModes) {
    auto btn = std::make_unique<ButtonPanelComp>(2, "TestButton");
    btn->SetParam("button_type", static_cast<int32_t>(1)); // Toggle

    EXPECT_FALSE(btn->GetState());
    btn->Click();
    EXPECT_TRUE(btn->GetState());
    btn->Click();
    EXPECT_FALSE(btn->GetState());

    CookContext ctx{};
    EXPECT_TRUE(btn->Cook(ctx));
    const auto& chanVal = btn->GetOutputPin("out_chan")->GetValue();
    ASSERT_TRUE(chanVal.Is<ChannelBuffer>());
    EXPECT_FLOAT_EQ(chanVal.Get<ChannelBuffer>().GetChannelData(0)[0], 0.0f);
}

TEST_F(PanelSystemTest, DialPanelCompAngleMath) {
    auto dial = std::make_unique<DialPanelComp>(3, "TestDial");
    dial->SetParam("min_value", 0.0f);
    dial->SetParam("max_value", 100.0f);
    dial->SetParam("sweep_angle", 270.0f);

    dial->SetValueInteractive(50.0f); // Mid-point
    EXPECT_NEAR(dial->GetAngleDegrees(), 0.0f, 0.001f);

    dial->SetValueInteractive(0.0f);  // Min-point
    EXPECT_NEAR(dial->GetAngleDegrees(), -135.0f, 0.001f);

    dial->SetValueInteractive(100.0f); // Max-point
    EXPECT_NEAR(dial->GetAngleDegrees(), 135.0f, 0.001f);

    CookContext ctx{};
    EXPECT_TRUE(dial->Cook(ctx));
}

TEST_F(PanelSystemTest, TextEntryPanelCompData) {
    auto textComp = std::make_unique<TextEntryPanelComp>(4, "TestText");
    textComp->SetText("Neo Realms Mainstage");

    CookContext ctx{};
    EXPECT_TRUE(textComp->Cook(ctx));

    const auto& dataVal = textComp->GetOutputPin("out_data")->GetValue();
    ASSERT_TRUE(dataVal.Is<DataTable>());
    const auto& table = dataVal.Get<DataTable>();
    EXPECT_EQ(table.GetRowCount(), 1u);
    EXPECT_EQ(table.GetCell(0, 0), "Neo Realms Mainstage");
}

TEST_F(PanelSystemTest, LoadLiveShowControlPanelSampleProject) {
    std::filesystem::path samplePath = "samples/07_live_show_control_panel/live_show_control_panel.nfp";
    ASSERT_TRUE(std::filesystem::exists(samplePath)) << "Missing file: " << samplePath;

    ProjectData proj;
    std::string err;
    bool success = ProjectSerializer::LoadFromFile(samplePath.string(), proj, &err);
    ASSERT_TRUE(success) << "Failed loading live_show_control_panel.nfp: " << err;

    EXPECT_EQ(proj.metadata.name, "Master Live Show Control Panel");
    EXPECT_GE(proj.rootGraph->GetNodes().size(), 8u);
    EXPECT_GE(proj.rootGraph->GetWires().size(), 3u);

    // Verify PanelComp and widgets exist
    bool hasPanel = false;
    bool hasSlider = false;
    bool hasButton = false;
    for (const auto& [id, node] : proj.rootGraph->GetNodes()) {
        if (node->GetTypeName() == "PanelComp") hasPanel = true;
        if (node->GetTypeName() == "SliderPanelComp") hasSlider = true;
        if (node->GetTypeName() == "ButtonPanelComp") hasButton = true;
    }
    EXPECT_TRUE(hasPanel);
    EXPECT_TRUE(hasSlider);
    EXPECT_TRUE(hasButton);
}
