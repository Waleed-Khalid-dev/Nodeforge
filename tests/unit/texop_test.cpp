#include <gtest/gtest.h>
#include "graph/Graph.h"
#include "graph/NodeRegistry.h"
#include "graph/CoreNodes.h"
#include "gpu/Device.h"
#include "gpu/TexturePool.h"
#include "operators/tex/NullTexOp.h"
#include "operators/tex/ConstantTexOp.h"
#include "operators/tex/NoiseTexOp.h"
#include "operators/tex/LoadImageTexOp.h"
#include "operators/tex/TransformTexOp.h"
#include "operators/tex/CompositeTexOp.h"
#include "operators/tex/BlurTexOp.h"
#include "operators/tex/LevelTexOp.h"
#include "operators/tex/ResolutionTexOp.h"
#include "operators/tex/ToWindowTexOp.h"

class TexOpTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_device = std::make_unique<gpu::Device>();
        m_device->Initialize(nullptr); // Headless Vulkan 1.3
        m_pool = std::make_unique<gpu::TexturePool>(m_device.get());

        m_context.frameIndex = 1;
        m_context.timeSeconds = 0.0;
        m_context.deltaTimeSeconds = 1.0 / 60.0;
        m_context.gpuDevice = m_device.get();
        m_context.texturePool = m_pool.get();
    }

    void TearDown() override {
        m_pool->Clear();
        m_device->Cleanup();
    }

    std::unique_ptr<gpu::Device> m_device;
    std::unique_ptr<gpu::TexturePool> m_pool;
    nf::CookContext m_context;
};

TEST_F(TexOpTest, NullTexOpPassThrough) {
    nf::Graph graph;
    auto* constOp = graph.CreateNode<nf::ConstantTexOp>("SourceConst");
    auto* nullOp = graph.CreateNode<nf::NullTexOp>("NullPass");

    graph.Connect(constOp->GetOutputPin("output"), nullOp->GetInputPin("input"));
    EXPECT_TRUE(graph.CookAll(m_context));

    EXPECT_NE(constOp->GetOutputTexture(), nullptr);
    EXPECT_EQ(nullOp->GetOutputTexture(), constOp->GetOutputTexture());
}

TEST_F(TexOpTest, ConstantTexOpDimensionsAndColor) {
    nf::Graph graph;
    auto* constOp = graph.CreateNode<nf::ConstantTexOp>("SolidRed");
    constOp->GetParams().Get("resolution_w")->SetValue(nf::PinValue(static_cast<int32_t>(640)));
    constOp->GetParams().Get("resolution_h")->SetValue(nf::PinValue(static_cast<int32_t>(480)));
    constOp->GetParams().Get("color")->SetValue(nf::PinValue(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)));

    EXPECT_TRUE(constOp->EnsureCooked(m_context));
    auto tex = constOp->GetOutputTexture();
    ASSERT_NE(tex, nullptr);
    EXPECT_EQ(tex->GetWidth(), 640u);
    EXPECT_EQ(tex->GetHeight(), 480u);
}

TEST_F(TexOpTest, NoiseTexOpGeneration) {
    nf::Graph graph;
    auto* noiseOp = graph.CreateNode<nf::NoiseTexOp>("NoiseGen");
    noiseOp->GetParams().Get("resolution_w")->SetValue(nf::PinValue(static_cast<int32_t>(512)));
    noiseOp->GetParams().Get("resolution_h")->SetValue(nf::PinValue(static_cast<int32_t>(512)));
    noiseOp->GetParams().Get("period")->SetValue(nf::PinValue(2.5f));
    noiseOp->GetParams().Get("octaves")->SetValue(nf::PinValue(static_cast<int32_t>(4)));

    EXPECT_TRUE(noiseOp->EnsureCooked(m_context));
    auto tex = noiseOp->GetOutputTexture();
    ASSERT_NE(tex, nullptr);
    EXPECT_EQ(tex->GetWidth(), 512u);
    EXPECT_EQ(tex->GetHeight(), 512u);
}

TEST_F(TexOpTest, LoadImageTexOpFallback) {
    nf::Graph graph;
    auto* loadOp = graph.CreateNode<nf::LoadImageTexOp>("ImageLoader");
    // Empty path produces checkerboard
    EXPECT_TRUE(loadOp->EnsureCooked(m_context));
    auto tex = loadOp->GetOutputTexture();
    ASSERT_NE(tex, nullptr);
    EXPECT_EQ(tex->GetWidth(), 256u);
    EXPECT_EQ(tex->GetHeight(), 256u);
}

TEST_F(TexOpTest, TransformTexOpParameters) {
    nf::Graph graph;
    auto* constOp = graph.CreateNode<nf::ConstantTexOp>("BaseColor");
    auto* transOp = graph.CreateNode<nf::TransformTexOp>("Transform");

    graph.Connect(constOp->GetOutputPin("output"), transOp->GetInputPin("input"));
    transOp->GetParams().Get("translate")->SetValue(nf::PinValue(glm::vec2(0.2f, -0.1f)));
    transOp->GetParams().Get("rotate")->SetValue(nf::PinValue(45.0f));
    transOp->GetParams().Get("scale")->SetValue(nf::PinValue(glm::vec2(1.5f, 1.5f)));

    EXPECT_TRUE(graph.CookAll(m_context));
    EXPECT_NE(transOp->GetOutputTexture(), nullptr);
}

TEST_F(TexOpTest, CompositeTexOpOperations) {
    nf::Graph graph;
    auto* bg = graph.CreateNode<nf::ConstantTexOp>("Background");
    bg->GetParams().Get("color")->SetValue(nf::PinValue(glm::vec4(0.1f, 0.2f, 0.3f, 1.0f)));

    auto* fg = graph.CreateNode<nf::ConstantTexOp>("Foreground");
    fg->GetParams().Get("color")->SetValue(nf::PinValue(glm::vec4(0.8f, 0.4f, 0.2f, 0.5f)));

    auto* comp = graph.CreateNode<nf::CompositeTexOp>("Composite");
    comp->GetParams().Get("operation")->SetValue(nf::PinValue(static_cast<int32_t>(1))); // Add
    comp->GetParams().Get("opacity")->SetValue(nf::PinValue(0.8f));

    graph.Connect(bg->GetOutputPin("output"), comp->GetInputPin("input1"));
    graph.Connect(fg->GetOutputPin("output"), comp->GetInputPin("input2"));

    EXPECT_TRUE(graph.CookAll(m_context));
    EXPECT_NE(comp->GetOutputTexture(), nullptr);
}

TEST_F(TexOpTest, BlurTexOpRadius) {
    nf::Graph graph;
    auto* constOp = graph.CreateNode<nf::ConstantTexOp>("ImageSource");
    auto* blurOp = graph.CreateNode<nf::BlurTexOp>("GaussianBlur");
    blurOp->GetParams().Get("radius")->SetValue(nf::PinValue(8.0f));

    graph.Connect(constOp->GetOutputPin("output"), blurOp->GetInputPin("input"));
    EXPECT_TRUE(graph.CookAll(m_context));
    EXPECT_NE(blurOp->GetOutputTexture(), nullptr);
}

TEST_F(TexOpTest, LevelTexOpAdjustments) {
    nf::Graph graph;
    auto* constOp = graph.CreateNode<nf::ConstantTexOp>("ImageSource");
    auto* levelOp = graph.CreateNode<nf::LevelTexOp>("Levels");
    levelOp->GetParams().Get("brightness")->SetValue(nf::PinValue(1.5f));
    levelOp->GetParams().Get("gamma")->SetValue(nf::PinValue(1.2f));
    levelOp->GetParams().Get("contrast")->SetValue(nf::PinValue(1.1f));

    graph.Connect(constOp->GetOutputPin("output"), levelOp->GetInputPin("input"));
    EXPECT_TRUE(graph.CookAll(m_context));
    EXPECT_NE(levelOp->GetOutputTexture(), nullptr);
}

TEST_F(TexOpTest, ResolutionTexOpResampling) {
    nf::Graph graph;
    auto* constOp = graph.CreateNode<nf::ConstantTexOp>("Original720p");
    constOp->GetParams().Get("resolution_w")->SetValue(nf::PinValue(static_cast<int32_t>(1280)));
    constOp->GetParams().Get("resolution_h")->SetValue(nf::PinValue(static_cast<int32_t>(720)));

    auto* resOp = graph.CreateNode<nf::ResolutionTexOp>("Resample1080p");
    resOp->GetParams().Get("resolution_w")->SetValue(nf::PinValue(static_cast<int32_t>(1920)));
    resOp->GetParams().Get("resolution_h")->SetValue(nf::PinValue(static_cast<int32_t>(1080)));
    resOp->GetParams().Get("fit_mode")->SetValue(nf::PinValue(static_cast<int32_t>(1))); // Fit H

    graph.Connect(constOp->GetOutputPin("output"), resOp->GetInputPin("input"));
    EXPECT_TRUE(graph.CookAll(m_context));

    auto tex = resOp->GetOutputTexture();
    ASSERT_NE(tex, nullptr);
    EXPECT_EQ(tex->GetWidth(), 1920u);
    EXPECT_EQ(tex->GetHeight(), 1080u);
}

TEST_F(TexOpTest, ToWindowTexOpForwarding) {
    nf::Graph graph;
    auto* constOp = graph.CreateNode<nf::ConstantTexOp>("Source");
    auto* windowOp = graph.CreateNode<nf::ToWindowTexOp>("DisplayWindow");

    graph.Connect(constOp->GetOutputPin("output"), windowOp->GetInputPin("input"));
    EXPECT_TRUE(graph.CookAll(m_context));
    EXPECT_EQ(windowOp->GetOutputTexture(), constOp->GetOutputTexture());
}
