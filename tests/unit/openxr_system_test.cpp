#include <gtest/gtest.h>
#include "xr/OpenXRRuntime.h"
#include "xr/OpenXRStereoMath.h"
#include "operators/tex/OpenXRHeadsetTexOp.h"
#include "operators/chan/OpenXRControllerChanOp.h"
#include "operators/chan/OpenXRHandTrackingChanOp.h"
#include "operators/comp/OpenXRCameraComp.h"
#include "project/ProjectSerializer.h"
#include "graph/CoreNodes.h"
#include <filesystem>

using namespace nf;
using namespace nf::project;

class OpenXRSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        RegisterCoreNodes(NodeRegistry::Instance());
        OpenXRRuntime::Instance().Initialize(true); // Headless emulated mode
    }
};

TEST_F(OpenXRSystemTest, StereoProjectionMatrixMath) {
    OpenXRFov fov;
    fov.angleLeft = -0.785398f;  // -45 deg
    fov.angleRight = 0.785398f;  // +45 deg
    fov.angleUp = 0.785398f;     // +45 deg
    fov.angleDown = -0.785398f;  // -45 deg

    glm::mat4 proj = OpenXRStereoMath::ComputeProjectionMatrix(fov, 0.1f, 100.0f);

    EXPECT_NEAR(proj[0][0], 1.0f, 0.01f);
    EXPECT_NEAR(proj[1][1], 1.0f, 0.01f);
    EXPECT_NEAR(proj[2][3], -1.0f, 0.001f);
}

TEST_F(OpenXRSystemTest, StereoViewMatrixIPD) {
    glm::vec3 headPos(0.0f, 1.7f, 0.0f);
    glm::quat headRot(1.0f, 0.0f, 0.0f, 0.0f);
    float ipd = 0.064f; // 64mm

    glm::mat4 viewLeft = OpenXRStereoMath::ComputeViewMatrix(headPos, headRot, true, ipd);
    glm::mat4 viewRight = OpenXRStereoMath::ComputeViewMatrix(headPos, headRot, false, ipd);

    // Left eye should be at x = -0.032, Right eye at x = +0.032
    EXPECT_NE(viewLeft[3][0], viewRight[3][0]);
    EXPECT_NEAR(viewLeft[3][0], 0.032f, 0.001f);
    EXPECT_NEAR(viewRight[3][0], -0.032f, 0.001f);
}

TEST_F(OpenXRSystemTest, OpenXRControllerChannelStreams) {
    auto ctrl = std::make_unique<OpenXRControllerChanOp>(1, "VRControllers");

    XRControllerState mockLeft;
    mockLeft.position = glm::vec3(-0.3f, 1.1f, -0.5f);
    mockLeft.trigger = 0.85f;
    mockLeft.primaryButton = true;
    OpenXRRuntime::Instance().SetEmulatedController(XRHand::Left, mockLeft);

    CookContext ctx{};
    EXPECT_TRUE(ctrl->Cook(ctx));

    const auto& outVal = ctrl->GetOutputPin("out_left")->GetValue();
    ASSERT_TRUE(outVal.Is<ChannelBuffer>());
    const auto& buf = outVal.Get<ChannelBuffer>();

    EXPECT_EQ(buf.GetChannelCount(), 15u);
    EXPECT_NEAR(buf.GetChannelData(0)[0], -0.3f, 0.01f); // tx
    EXPECT_NEAR(buf.GetChannelData(7)[0], 0.85f, 0.01f); // trigger
    EXPECT_NEAR(buf.GetChannelData(11)[0], 1.0f, 0.01f); // primaryButton
}

TEST_F(OpenXRSystemTest, OpenXRHandTrackingJointStreams) {
    auto hands = std::make_unique<OpenXRHandTrackingChanOp>(2, "HandTracker");

    XRHandTrackingState mockRight;
    mockRight.pinchStrength = 0.92f;
    mockRight.grabStrength = 0.45f;
    mockRight.palmNormal = glm::vec3(0.0f, -1.0f, 0.0f);
    mockRight.joints.resize(26);
    mockRight.joints[0].position = glm::vec3(0.3f, 1.2f, -0.4f); // Wrist
    OpenXRRuntime::Instance().SetEmulatedHandTracking(XRHand::Right, mockRight);

    CookContext ctx{};
    EXPECT_TRUE(hands->Cook(ctx));

    const auto& gestVal = hands->GetOutputPin("out_gestures")->GetValue();
    ASSERT_TRUE(gestVal.Is<ChannelBuffer>());
    const auto& gestBuf = gestVal.Get<ChannelBuffer>();
    EXPECT_NEAR(gestBuf.GetChannelData(0)[0], 0.92f, 0.01f); // pinch

    const auto& skelVal = hands->GetOutputPin("out_skeleton")->GetValue();
    ASSERT_TRUE(skelVal.Is<ChannelBuffer>());
    const auto& skelBuf = skelVal.Get<ChannelBuffer>();
    EXPECT_EQ(skelBuf.GetChannelCount(), 26u * 3u);
}

TEST_F(OpenXRSystemTest, OpenXRHeadsetTexOpCooking) {
    auto headset = std::make_unique<OpenXRHeadsetTexOp>(3, "VRHeadset");
    headset->SetParam("ipd_mm", 68.0f);

    CookContext ctx{};
    EXPECT_TRUE(headset->Cook(ctx));
    EXPECT_NEAR(OpenXRRuntime::Instance().GetIPD(), 0.068f, 0.001f);
}

TEST_F(OpenXRSystemTest, LoadOpenXRHolodeckSampleProject) {
    std::filesystem::path samplePath = "samples/10_openxr_immersive_holodeck/openxr_immersive_holodeck.nfp";
    ASSERT_TRUE(std::filesystem::exists(samplePath)) << "Missing file: " << samplePath;

    ProjectData proj;
    std::string err;
    bool success = ProjectSerializer::LoadFromFile(samplePath.string(), proj, &err);
    ASSERT_TRUE(success) << "Failed loading openxr_immersive_holodeck.nfp: " << err;

    EXPECT_EQ(proj.metadata.name, "Interactive VR Spatial Holodeck & Kinetic Gesture Arena");
    EXPECT_GE(proj.rootGraph->GetNodes().size(), 10u);
    EXPECT_GE(proj.rootGraph->GetWires().size(), 7u);

    bool hasHeadset = false;
    bool hasControllers = false;
    for (const auto& [id, node] : proj.rootGraph->GetNodes()) {
        if (node->GetTypeName() == "OpenXRHeadsetTexOp") hasHeadset = true;
        if (node->GetTypeName() == "OpenXRControllerChanOp") hasControllers = true;
    }
    EXPECT_TRUE(hasHeadset);
    EXPECT_TRUE(hasControllers);
}
