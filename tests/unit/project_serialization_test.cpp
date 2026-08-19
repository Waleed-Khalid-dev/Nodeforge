#include <gtest/gtest.h>
#include "project/ProjectSerializer.h"
#include "project/PathUtils.h"
#include "project/AutosaveManager.h"
#include "graph/CoreNodes.h"
#include "param/Parameter.h"
#include <filesystem>

using namespace nf;
using namespace nf::project;

class ProjectSerializationTest : public ::testing::Test {
protected:
    void SetUp() override {
        RegisterCoreNodes(NodeRegistry::Instance());
        m_tempDir = std::filesystem::temp_directory_path() / "nodeforge_test_proj";
        std::filesystem::create_directories(m_tempDir);
    }

    void TearDown() override {
        std::error_code ec;
        std::filesystem::remove_all(m_tempDir, ec);
    }

    std::filesystem::path m_tempDir;
};

TEST_F(ProjectSerializationTest, RelativePathResolution) {
    std::string projDir = "D:/Projects/Show2026";
    std::string absPath = "D:/Projects/Show2026/assets/textures/noise.png";

    std::string rel = PathUtils::ToProjectRelative(absPath, projDir);
    EXPECT_EQ(rel, "assets/textures/noise.png");

    std::string backToAbs = PathUtils::ToAbsolute(rel, projDir);
    EXPECT_EQ(backToAbs, "D:/Projects/Show2026/assets/textures/noise.png");
}

TEST_F(ProjectSerializationTest, SaveAndLoadFullGraphWithUI) {
    ProjectData originalProj;
    originalProj.metadata.name = "TestShow";
    originalProj.metadata.author = "NeoRealms";
    originalProj.timeline.fps = 60.0f;
    originalProj.timeline.bpm = 128.0f;
    originalProj.viewport.pan = glm::vec2(150.0f, -80.0f);
    originalProj.viewport.zoom = 1.25f;

    auto n1 = NodeRegistry::Instance().CreateNode("ConstantChanOp", 101, "Const1");
    n1->SetParam("sample_rate", 120.0f);
    originalProj.rootGraph->AddNode(std::move(n1));
    originalProj.nodePositions[101] = glm::vec2(200.0f, 300.0f);

    auto n2 = NodeRegistry::Instance().CreateNode("MathChanOp", 102, "Math1");
    auto* pScalar = n2->GetParams().Get("scalar");
    if (pScalar) {
        pScalar->SetExpression("me.time * 2.0");
    }
    originalProj.rootGraph->AddNode(std::move(n2));
    originalProj.nodePositions[102] = glm::vec2(500.0f, 300.0f);

    // Connect wire
    Pin* outPin = originalProj.rootGraph->GetNode(101)->GetOutputPin("output");
    Pin* inPin = originalProj.rootGraph->GetNode(102)->GetInputPin("input");
    ASSERT_NE(outPin, nullptr);
    ASSERT_NE(inPin, nullptr);
    EXPECT_TRUE(originalProj.rootGraph->Connect(outPin, inPin));

    // Save to disk
    std::string testFile = (m_tempDir / "show.nfp").string();
    std::string saveErr;
    EXPECT_TRUE(ProjectSerializer::SaveToFile(testFile, originalProj, &saveErr)) << saveErr;
    EXPECT_TRUE(std::filesystem::exists(testFile));

    // Load from disk
    ProjectData loadedProj;
    std::string loadErr;
    EXPECT_TRUE(ProjectSerializer::LoadFromFile(testFile, loadedProj, &loadErr)) << loadErr;

    // Verify metadata & timeline
    EXPECT_EQ(loadedProj.metadata.name, "TestShow");
    EXPECT_EQ(loadedProj.metadata.author, "NeoRealms");
    EXPECT_FLOAT_EQ(loadedProj.timeline.fps, 60.0f);
    EXPECT_FLOAT_EQ(loadedProj.timeline.bpm, 128.0f);
    EXPECT_FLOAT_EQ(loadedProj.viewport.pan.x, 150.0f);
    EXPECT_FLOAT_EQ(loadedProj.viewport.pan.y, -80.0f);
    EXPECT_FLOAT_EQ(loadedProj.viewport.zoom, 1.25f);

    // Verify nodes & positions
    ASSERT_EQ(loadedProj.rootGraph->GetNodes().size(), 2u);
    EXPECT_EQ(loadedProj.nodePositions[101].x, 200.0f);
    EXPECT_EQ(loadedProj.nodePositions[101].y, 300.0f);
    EXPECT_EQ(loadedProj.nodePositions[102].x, 500.0f);
    EXPECT_EQ(loadedProj.nodePositions[102].y, 300.0f);

    // Verify parameters & expressions
    Node* loadedN1 = loadedProj.rootGraph->GetNode(101);
    ASSERT_NE(loadedN1, nullptr);
    EXPECT_FLOAT_EQ(loadedN1->GetParam("sample_rate").Get<float>(), 120.0f);

    Node* loadedN2 = loadedProj.rootGraph->GetNode(102);
    ASSERT_NE(loadedN2, nullptr);
    Parameter* loadedScalar = loadedN2->GetParams().Get("scalar");
    ASSERT_NE(loadedScalar, nullptr);
    EXPECT_EQ(loadedScalar->GetMode(), ParamMode::Expression);
    EXPECT_EQ(loadedScalar->GetExpression(), "me.time * 2.0");

    // Verify wire connectivity
    ASSERT_EQ(loadedProj.rootGraph->GetWires().size(), 1u);
    Pin* lOut = loadedN1->GetOutputPin("output");
    Pin* lIn = loadedN2->GetInputPin("input");
    ASSERT_NE(lOut, nullptr);
    ASSERT_NE(lIn, nullptr);
    EXPECT_TRUE(lIn->IsConnected());
    EXPECT_EQ(lIn->GetConnectedSource(), lOut);
}

TEST_F(ProjectSerializationTest, AutosaveAndCrashRecovery) {
    ProjectData proj;
    proj.filePath = (m_tempDir / "active_show.nfp").string();
    proj.isDirty = true;

    auto n = NodeRegistry::Instance().CreateNode("ConstantTexOp", 1, "ConstTex1");
    proj.rootGraph->AddNode(std::move(n));

    AutosaveManager autosave(1.0f); // 1-second interval
    EXPECT_TRUE(autosave.PerformAutosave(proj));

    std::string autosavePath;
    EXPECT_TRUE(autosave.HasNewerAutosave(proj.filePath, autosavePath));
    EXPECT_TRUE(std::filesystem::exists(autosavePath));

    // Recovery test
    ProjectData recoveredProj;
    std::string err;
    EXPECT_TRUE(ProjectSerializer::LoadFromFile(autosavePath, recoveredProj, &err)) << err;
    EXPECT_EQ(recoveredProj.rootGraph->GetNodes().size(), 1u);

    // Discard test
    autosave.DiscardAutosave(proj.filePath);
    EXPECT_FALSE(std::filesystem::exists(autosavePath));
}
