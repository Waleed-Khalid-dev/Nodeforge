#include <gtest/gtest.h>
#include "project/ProjectSerializer.h"
#include "project/ComponentSerializer.h"
#include "graph/CoreNodes.h"
#include "graph/Graph.h"
#include <filesystem>

using namespace nf;
using namespace nf::project;

class WorkflowPackTest : public ::testing::Test {
protected:
    void SetUp() override {
        RegisterCoreNodes(NodeRegistry::Instance());
    }
};

TEST_F(WorkflowPackTest, LoadFacadeMappingSample) {
    std::filesystem::path samplePath = "samples/01_facade_mapping/facade_mapping.nfp";
    ASSERT_TRUE(std::filesystem::exists(samplePath)) << "Missing file: " << samplePath;

    ProjectData proj;
    std::string err;
    bool success = ProjectSerializer::LoadFromFile(samplePath.string(), proj, &err);
    ASSERT_TRUE(success) << "Failed to load facade_mapping.nfp: " << err;

    EXPECT_EQ(proj.metadata.name, "Flagship Façade Mapping Show");
    EXPECT_FLOAT_EQ(proj.timeline.fps, 60.0f);
    EXPECT_GE(proj.rootGraph->GetNodes().size(), 9u);
    EXPECT_GE(proj.rootGraph->GetWires().size(), 8u);

    // Verify key nodes exist
    bool hasTime = false;
    bool hasLFO = false;
    bool hasNoise = false;
    bool hasWarpLeft = false;
    bool hasWarpRight = false;
    bool hasProjOut1 = false;
    bool hasProjOut2 = false;

    for (const auto& [id, node] : proj.rootGraph->GetNodes()) {
        if (node->GetName() == "master_clock") hasTime = true;
        if (node->GetName() == "anim_lfo") hasLFO = true;
        if (node->GetName() == "base_pattern") hasNoise = true;
        if (node->GetName() == "warp_blend_left") hasWarpLeft = true;
        if (node->GetName() == "warp_blend_right") hasWarpRight = true;
        if (node->GetName() == "projector_out_1") hasProjOut1 = true;
        if (node->GetName() == "projector_out_2") hasProjOut2 = true;
    }

    EXPECT_TRUE(hasTime);
    EXPECT_TRUE(hasLFO);
    EXPECT_TRUE(hasNoise);
    EXPECT_TRUE(hasWarpLeft);
    EXPECT_TRUE(hasWarpRight);
    EXPECT_TRUE(hasProjOut1);
    EXPECT_TRUE(hasProjOut2);
}

TEST_F(WorkflowPackTest, LoadInteractiveFloorplanSample) {
    std::filesystem::path samplePath = "samples/02_interactive_floorplan/interactive_floorplan.nfp";
    ASSERT_TRUE(std::filesystem::exists(samplePath)) << "Missing file: " << samplePath;

    ProjectData proj;
    std::string err;
    bool success = ProjectSerializer::LoadFromFile(samplePath.string(), proj, &err);
    ASSERT_TRUE(success) << "Failed to load interactive_floorplan.nfp: " << err;

    EXPECT_EQ(proj.metadata.name, "Interactive Walkable Floor Plan 3D");
    EXPECT_GE(proj.rootGraph->GetNodes().size(), 11u);
    EXPECT_GE(proj.rootGraph->GetWires().size(), 10u);

    // Verify 3D pipeline nodes exist
    bool hasOSC = false;
    bool hasFloor = false;
    bool hasAvatar = false;
    bool hasCamera = false;
    bool hasRender = false;

    for (const auto& [id, node] : proj.rootGraph->GetNodes()) {
        if (node->GetName() == "osc_sensor_in") hasOSC = true;
        if (node->GetName() == "floor_mesh") hasFloor = true;
        if (node->GetName() == "avatar_sphere") hasAvatar = true;
        if (node->GetName() == "main_camera") hasCamera = true;
        if (node->GetName() == "render_view") hasRender = true;
    }

    EXPECT_TRUE(hasOSC);
    EXPECT_TRUE(hasFloor);
    EXPECT_TRUE(hasAvatar);
    EXPECT_TRUE(hasCamera);
    EXPECT_TRUE(hasRender);
}

TEST_F(WorkflowPackTest, LoadAudiovisualStageSample) {
    std::filesystem::path samplePath = "samples/03_audiovisual_stage/audiovisual_stage.nfp";
    ASSERT_TRUE(std::filesystem::exists(samplePath)) << "Missing file: " << samplePath;

    ProjectData proj;
    std::string err;
    bool success = ProjectSerializer::LoadFromFile(samplePath.string(), proj, &err);
    ASSERT_TRUE(success) << "Failed to load audiovisual_stage.nfp: " << err;

    EXPECT_EQ(proj.metadata.name, "Generative Audio-Visual Stage Synthesizer");
    EXPECT_GE(proj.rootGraph->GetNodes().size(), 9u);
    EXPECT_GE(proj.rootGraph->GetWires().size(), 7u);

    bool hasAudio = false;
    bool hasMIDI = false;
    bool hasChanToTex = false;
    bool hasNoise = false;
    bool hasLevel = false;

    for (const auto& [id, node] : proj.rootGraph->GetNodes()) {
        if (node->GetName() == "audio_stream") hasAudio = true;
        if (node->GetName() == "midi_faders") hasMIDI = true;
        if (node->GetName() == "audio_to_texture") hasChanToTex = true;
        if (node->GetName() == "kaleido_noise") hasNoise = true;
        if (node->GetName() == "stage_level") hasLevel = true;
    }

    EXPECT_TRUE(hasAudio);
    EXPECT_TRUE(hasMIDI);
    EXPECT_TRUE(hasChanToTex);
    EXPECT_TRUE(hasNoise);
    EXPECT_TRUE(hasLevel);
}

TEST_F(WorkflowPackTest, LoadDMXShowControlSample) {
    std::filesystem::path samplePath = "samples/04_dmx_showcontrol/dmx_showcontrol.nfp";
    ASSERT_TRUE(std::filesystem::exists(samplePath)) << "Missing file: " << samplePath;

    ProjectData proj;
    std::string err;
    bool success = ProjectSerializer::LoadFromFile(samplePath.string(), proj, &err);
    ASSERT_TRUE(success) << "Failed to load dmx_showcontrol.nfp: " << err;

    EXPECT_EQ(proj.metadata.name, "Art-Net DMX512 & Show Control System");
    EXPECT_GE(proj.rootGraph->GetNodes().size(), 7u);
    EXPECT_GE(proj.rootGraph->GetWires().size(), 3u);

    bool hasLFO = false;
    bool hasDMX = false;
    bool hasSerial = false;
    bool hasKeys = false;

    for (const auto& [id, node] : proj.rootGraph->GetNodes()) {
        if (node->GetName() == "chase_lfo") hasLFO = true;
        if (node->GetName() == "artnet_dmx_out") hasDMX = true;
        if (node->GetName() == "arduino_serial") hasSerial = true;
        if (node->GetName() == "cue_keys") hasKeys = true;
    }

    EXPECT_TRUE(hasLFO);
    EXPECT_TRUE(hasDMX);
    EXPECT_TRUE(hasSerial);
    EXPECT_TRUE(hasKeys);
}

TEST_F(WorkflowPackTest, ImportReusableComponentTemplates) {
    Graph graph;
    std::string err;

    // 1. DualProjectorWarpRig
    std::string warpRigPath = "templates/dual_projector_warp_rig.nfc";
    ASSERT_TRUE(std::filesystem::exists(warpRigPath));
    ContainerComp* warpComp = ComponentSerializer::ImportComponent(warpRigPath, &graph, glm::vec2(0.0f), &err);
    ASSERT_NE(warpComp, nullptr) << "Failed importing dual_projector_warp_rig.nfc: " << err;
    EXPECT_EQ(warpComp->GetName(), "DualProjectorWarpRig");
    ASSERT_NE(warpComp->GetInnerGraph(), nullptr);
    EXPECT_EQ(warpComp->GetInnerGraph()->GetNodes().size(), 5u);

    // 2. OSCGestureTracker
    std::string oscTrackerPath = "templates/osc_gesture_tracker.nfc";
    ASSERT_TRUE(std::filesystem::exists(oscTrackerPath));
    ContainerComp* oscComp = ComponentSerializer::ImportComponent(oscTrackerPath, &graph, glm::vec2(100.0f), &err);
    ASSERT_NE(oscComp, nullptr) << "Failed importing osc_gesture_tracker.nfc: " << err;
    EXPECT_EQ(oscComp->GetName(), "OSCGestureTracker");
    ASSERT_NE(oscComp->GetInnerGraph(), nullptr);
    EXPECT_EQ(oscComp->GetInnerGraph()->GetNodes().size(), 3u);

    // 3. ArtNetDMXChase
    std::string dmxChasePath = "templates/artnet_dmx_chase.nfc";
    ASSERT_TRUE(std::filesystem::exists(dmxChasePath));
    ContainerComp* dmxComp = ComponentSerializer::ImportComponent(dmxChasePath, &graph, glm::vec2(200.0f), &err);
    ASSERT_NE(dmxComp, nullptr) << "Failed importing artnet_dmx_chase.nfc: " << err;
    EXPECT_EQ(dmxComp->GetName(), "ArtNetDMXChase");
    ASSERT_NE(dmxComp->GetInnerGraph(), nullptr);
    EXPECT_EQ(dmxComp->GetInnerGraph()->GetNodes().size(), 4u);
}
