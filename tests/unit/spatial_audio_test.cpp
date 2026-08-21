#include <gtest/gtest.h>
#include "operators/chan/AudioSpatializerChanOp.h"
#include "operators/chan/AmbisonicDecodeChanOp.h"
#include "operators/comp/AudioEmitterComp.h"
#include "operators/comp/AudioListenerComp.h"
#include "project/ProjectSerializer.h"
#include "graph/CoreNodes.h"
#include <filesystem>

using namespace nf;
using namespace nf::project;

class SpatialAudioTest : public ::testing::Test {
protected:
    void SetUp() override {
        RegisterCoreNodes(NodeRegistry::Instance());
    }
};

TEST_F(SpatialAudioTest, AmbisonicsBFormatEncoding) {
    auto spatializer = std::make_unique<AudioSpatializerChanOp>(1, "BFormatSpatializer");
    spatializer->SetParam("spatial_mode", static_cast<int32_t>(0)); // Ambisonics B-Format
    spatializer->SetParam("attenuation_rolloff", 0.0f); // Disable distance falloff for pure angle check

    // Sound positioned directly in front (Z = -10, X = 0, Y = 0)
    spatializer->SetParam("emitter_pos", glm::vec3(0.0f, 0.0f, -10.0f));
    spatializer->SetParam("listener_pos", glm::vec3(0.0f, 0.0f, 0.0f));

    CookContext ctx{};
    EXPECT_TRUE(spatializer->Cook(ctx));

    const auto& outVal = spatializer->GetOutputPin("out_audio")->GetValue();
    ASSERT_TRUE(outVal.Is<ChannelBuffer>());
    const auto& buf = outVal.Get<ChannelBuffer>();
    EXPECT_EQ(buf.GetChannelCount(), 4u);
    EXPECT_EQ(buf.GetChannelNames()[0], "W");
    EXPECT_EQ(buf.GetChannelNames()[1], "X");
    EXPECT_EQ(buf.GetChannelNames()[2], "Y");
    EXPECT_EQ(buf.GetChannelNames()[3], "Z");

    EXPECT_NEAR(buf.GetChannelData(0)[0], 0.7071f, 0.01f); // W = 1 / sqrt(2)
    EXPECT_NEAR(buf.GetChannelData(1)[0], 1.0f, 0.01f);    // X (front) = 1.0
    EXPECT_NEAR(buf.GetChannelData(2)[0], 0.0f, 0.01f);    // Y (left/right) = 0.0
    EXPECT_NEAR(buf.GetChannelData(3)[0], 0.0f, 0.01f);    // Z (up/down) = 0.0
}

TEST_F(SpatialAudioTest, VBAPMultiSpeakerGains) {
    auto spatializer = std::make_unique<AudioSpatializerChanOp>(2, "VBAPSpatializer");
    spatializer->SetParam("spatial_mode", static_cast<int32_t>(1)); // VBAP Multi-Speaker
    spatializer->SetParam("speaker_layout", static_cast<int32_t>(2)); // 5.1 Surround
    spatializer->SetParam("attenuation_rolloff", 0.0f);

    // Sound positioned directly Center (0 deg)
    spatializer->SetParam("emitter_pos", glm::vec3(0.0f, 0.0f, -5.0f));

    CookContext ctx{};
    EXPECT_TRUE(spatializer->Cook(ctx));

    const auto& outVal = spatializer->GetOutputPin("out_audio")->GetValue();
    ASSERT_TRUE(outVal.Is<ChannelBuffer>());
    const auto& buf = outVal.Get<ChannelBuffer>();
    EXPECT_EQ(buf.GetChannelCount(), 6u);
    // Center channel should have highest gain
    EXPECT_GT(buf.GetChannelData(2)[0], buf.GetChannelData(4)[0]); // C > Ls
}

TEST_F(SpatialAudioTest, AmbisonicDecodingToSurround) {
    auto decoder = std::make_unique<AmbisonicDecodeChanOp>(3, "AtmosDecoder");
    decoder->SetParam("target_layout", static_cast<int32_t>(3)); // 7.1.4 Atmos Dome

    // Supply 4-channel B-Format buffer
    ChannelBuffer bformat({"W", "X", "Y", "Z"}, 1);
    bformat.GetChannelData(0)[0] = 0.7071f;
    bformat.GetChannelData(1)[0] = 1.0f;
    bformat.GetChannelData(2)[0] = 0.0f;
    bformat.GetChannelData(3)[0] = 0.5f;

    decoder->GetInputPin("in_bformat")->SetValue(PinValue(bformat));

    CookContext ctx{};
    EXPECT_TRUE(decoder->Cook(ctx));

    const auto& outVal = decoder->GetOutputPin("out_speakers")->GetValue();
    ASSERT_TRUE(outVal.Is<ChannelBuffer>());
    const auto& spkBuf = outVal.Get<ChannelBuffer>();
    EXPECT_EQ(spkBuf.GetChannelCount(), 12u);
}

TEST_F(SpatialAudioTest, AudioEmitterAndListenerIntegration) {
    auto emitter = std::make_unique<AudioEmitterComp>(4, "SceneEmitter");
    emitter->SetParam("translate", glm::vec3(15.0f, 0.0f, -20.0f));
    emitter->SetParam("radius", 3.0f);

    auto listener = std::make_unique<AudioListenerComp>(5, "SceneListener");
    listener->SetParam("position", glm::vec3(0.0f, 0.0f, 0.0f));

    CookContext ctx{};
    EXPECT_TRUE(emitter->Cook(ctx));
    EXPECT_TRUE(listener->Cook(ctx));

    const auto& stateVal = listener->GetOutputPin("out_state")->GetValue();
    ASSERT_TRUE(stateVal.Is<ChannelBuffer>());
    EXPECT_EQ(stateVal.Get<ChannelBuffer>().GetChannelCount(), 5u);
}

TEST_F(SpatialAudioTest, LoadSpatialAudioDomeSampleProject) {
    std::filesystem::path samplePath = "samples/08_spatial_audio_dome/spatial_audio_dome.nfp";
    ASSERT_TRUE(std::filesystem::exists(samplePath)) << "Missing file: " << samplePath;

    ProjectData proj;
    std::string err;
    bool success = ProjectSerializer::LoadFromFile(samplePath.string(), proj, &err);
    ASSERT_TRUE(success) << "Failed loading spatial_audio_dome.nfp: " << err;

    EXPECT_EQ(proj.metadata.name, "Immersive 3D Spatial Audio & Planetarium Dome");
    EXPECT_GE(proj.rootGraph->GetNodes().size(), 10u);
    EXPECT_GE(proj.rootGraph->GetWires().size(), 7u);

    // Verify spatializer and decoder nodes exist
    bool hasSpatializer = false;
    bool hasDecoder = false;
    for (const auto& [id, node] : proj.rootGraph->GetNodes()) {
        if (node->GetTypeName() == "AudioSpatializerChanOp") hasSpatializer = true;
        if (node->GetTypeName() == "AmbisonicDecodeChanOp") hasDecoder = true;
    }
    EXPECT_TRUE(hasSpatializer);
    EXPECT_TRUE(hasDecoder);
}
