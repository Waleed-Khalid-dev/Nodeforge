#include <gtest/gtest.h>
#include "ai/PoseKeypoints.h"
#include "ai/ONNXInferenceEngine.h"
#include "operators/tex/NeuralStyleTexOp.h"
#include "operators/tex/ONNXInferenceTexOp.h"
#include "operators/tex/SegmentationMaskTexOp.h"
#include "operators/chan/PoseEstimationChanOp.h"
#include "project/ProjectSerializer.h"
#include "graph/CoreNodes.h"
#include <filesystem>

using namespace nf;
using namespace nf::project;

class AINeuralTest : public ::testing::Test {
protected:
    void SetUp() override {
        RegisterCoreNodes(NodeRegistry::Instance());
        ONNXInferenceEngine::Instance().Initialize(true);
    }
};

TEST_F(AINeuralTest, NCHWTensorConversionAndReconstruction) {
    const int w = 4;
    const int h = 4;
    std::vector<uint8_t> rgba(w * h * 4, 128);

    auto nchw = ONNXInferenceEngine::ConvertRGBAtoNCHW(rgba.data(), w, h);
    EXPECT_EQ(nchw.size(), static_cast<size_t>(w * h * 3));

    std::vector<uint8_t> reconstructed(w * h * 4, 0);
    ONNXInferenceEngine::ConvertNCHWtoRGBA(nchw.data(), w, h, reconstructed.data());

    for (size_t i = 0; i < rgba.size(); ++i) {
        if (i % 4 == 3) {
            EXPECT_EQ(reconstructed[i], 255); // Alpha
        } else {
            EXPECT_NEAR(reconstructed[i], rgba[i], 2); // Minor quantization delta
        }
    }
}

TEST_F(AINeuralTest, NeuralStyleTransferPresets) {
    glm::vec4 baseColor(0.5f, 0.5f, 0.5f, 1.0f);

    glm::vec4 styledCandy = ONNXInferenceEngine::Instance().ApplyStylePresetColor(baseColor, 0, 1.0f);
    EXPECT_NE(styledCandy, baseColor);

    glm::vec4 styledCyberpunk = ONNXInferenceEngine::Instance().ApplyStylePresetColor(baseColor, 4, 1.0f);
    EXPECT_GT(styledCyberpunk.r, baseColor.r);

    glm::vec4 blendHalf = ONNXInferenceEngine::Instance().ApplyStylePresetColor(baseColor, 4, 0.5f);
    EXPECT_NEAR(blendHalf.r, (baseColor.r + styledCyberpunk.r) * 0.5f, 0.05f);
}

TEST_F(AINeuralTest, PoseEstimation17Keypoints) {
    Pose17 pose = ONNXInferenceEngine::Instance().EstimatePose(1.0f);
    EXPECT_EQ(pose.keypoints.size(), 17u);

    // Nose
    EXPECT_NEAR(pose.keypoints[0].position.y, 0.2f, 0.05f);
    EXPECT_GT(pose.keypoints[0].confidence, 0.8f);

    // Left & Right Wrists
    EXPECT_GT(pose.keypoints[9].position.y, 0.4f);
    EXPECT_GT(pose.keypoints[10].position.y, 0.4f);
}

TEST_F(AINeuralTest, SegmentationMaskMatteGeneration) {
    float centerAlpha = ONNXInferenceEngine::Instance().ComputeSegmentationAlpha(glm::vec2(0.5f, 0.5f), 0.0f, 0.5f);
    EXPECT_EQ(centerAlpha, 1.0f); // Center subject is foreground

    float cornerAlpha = ONNXInferenceEngine::Instance().ComputeSegmentationAlpha(glm::vec2(0.0f, 0.0f), 0.0f, 0.5f);
    EXPECT_EQ(cornerAlpha, 0.0f); // Outer boundary is background
}

TEST_F(AINeuralTest, NeuralStyleTexOpCooking) {
    auto styler = std::make_unique<NeuralStyleTexOp>(1, "TestStyler");
    styler->SetParam("style_preset", static_cast<int32_t>(4));
    styler->SetParam("style_weight", 0.8f);

    CookContext ctx{};
    EXPECT_TRUE(styler->Cook(ctx));
}

TEST_F(AINeuralTest, PoseEstimationChanOpCooking) {
    auto poseTracker = std::make_unique<PoseEstimationChanOp>(2, "TestPoseTracker");
    poseTracker->SetParam("confidence_threshold", 0.6f);

    CookContext ctx{};
    ctx.timeSeconds = 2.5;
    EXPECT_TRUE(poseTracker->Cook(ctx));

    const auto& kpVal = poseTracker->GetOutputPin("out_keypoints")->GetValue();
    ASSERT_TRUE(kpVal.Is<ChannelBuffer>());
    const auto& kpBuf = kpVal.Get<ChannelBuffer>();
    EXPECT_EQ(kpBuf.GetChannelCount(), 34u);

    const auto& confVal = poseTracker->GetOutputPin("out_confidence")->GetValue();
    ASSERT_TRUE(confVal.Is<ChannelBuffer>());
    const auto& confBuf = confVal.Get<ChannelBuffer>();
    EXPECT_EQ(confBuf.GetChannelCount(), 17u);
}

TEST_F(AINeuralTest, LoadAIDigitalTwinSampleProject) {
    std::filesystem::path samplePath = "samples/13_ai_interactive_digital_twin/ai_interactive_digital_twin.nfp";
    ASSERT_TRUE(std::filesystem::exists(samplePath)) << "Missing file: " << samplePath;

    ProjectData proj;
    std::string err;
    bool success = ProjectSerializer::LoadFromFile(samplePath.string(), proj, &err);
    ASSERT_TRUE(success) << "Failed loading ai_interactive_digital_twin.nfp: " << err;

    EXPECT_EQ(proj.metadata.name, "Live AI Kinetic Digital Twin & Cyberpunk Style Arena");
    EXPECT_GE(proj.rootGraph->GetNodes().size(), 8u);
    EXPECT_GE(proj.rootGraph->GetWires().size(), 4u);

    bool hasStyler = false;
    bool hasMatting = false;
    bool hasPose = false;
    for (const auto& [id, node] : proj.rootGraph->GetNodes()) {
        if (node->GetTypeName() == "NeuralStyleTexOp") hasStyler = true;
        if (node->GetTypeName() == "SegmentationMaskTexOp") hasMatting = true;
        if (node->GetTypeName() == "PoseEstimationChanOp") hasPose = true;
    }
    EXPECT_TRUE(hasStyler);
    EXPECT_TRUE(hasMatting);
    EXPECT_TRUE(hasPose);
}
