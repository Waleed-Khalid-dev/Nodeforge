#include <gtest/gtest.h>
#include "graph/Graph.h"
#include "gpu/Device.h"
#include "gpu/TexturePool.h"
#include "operators/tex/ConstantTexOp.h"
#include "operators/tex/BlurTexOp.h"
#include "operators/tex/CompositeTexOp.h"
#include "operators/tex/LevelTexOp.h"
#include "operators/tex/ToWindowTexOp.h"

TEST(TexOpIntegrationTest, EndToEndPipelineExecution) {
    auto device = std::make_unique<gpu::Device>();
    ASSERT_TRUE(device->Initialize(nullptr));

    auto pool = std::make_unique<gpu::TexturePool>(device.get());

    nf::CookContext ctx{};
    ctx.frameIndex = 1;
    ctx.timeSeconds = 0.0;
    ctx.gpuDevice = device.get();
    ctx.texturePool = pool.get();

    nf::Graph graph;

    // Node 1: Solid background (Blue)
    auto* bg = graph.CreateNode<nf::ConstantTexOp>("Background");
    bg->GetParams().Get("resolution_w")->SetValue(nf::PinValue(static_cast<int32_t>(1280)));
    bg->GetParams().Get("resolution_h")->SetValue(nf::PinValue(static_cast<int32_t>(720)));
    bg->GetParams().Get("color")->SetValue(nf::PinValue(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)));

    // Node 2: Foreground solid (Red)
    auto* fg = graph.CreateNode<nf::ConstantTexOp>("Foreground");
    fg->GetParams().Get("resolution_w")->SetValue(nf::PinValue(static_cast<int32_t>(1280)));
    fg->GetParams().Get("resolution_h")->SetValue(nf::PinValue(static_cast<int32_t>(720)));
    fg->GetParams().Get("color")->SetValue(nf::PinValue(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)));

    // Node 3: Blur foreground
    auto* blur = graph.CreateNode<nf::BlurTexOp>("BlurFG");
    blur->GetParams().Get("radius")->SetValue(nf::PinValue(4.0f));
    graph.Connect(fg->GetOutputPin("output"), blur->GetInputPin("input"));

    // Node 4: Composite (Add)
    auto* comp = graph.CreateNode<nf::CompositeTexOp>("CompositeLayers");
    comp->GetParams().Get("operation")->SetValue(nf::PinValue(static_cast<int32_t>(1))); // Add
    comp->GetParams().Get("opacity")->SetValue(nf::PinValue(1.0f));
    graph.Connect(bg->GetOutputPin("output"), comp->GetInputPin("input1"));
    graph.Connect(blur->GetOutputPin("output"), comp->GetInputPin("input2"));

    // Node 5: Levels (Brightness adjustments)
    auto* level = graph.CreateNode<nf::LevelTexOp>("PostLevels");
    level->GetParams().Get("brightness")->SetValue(nf::PinValue(1.0f));
    level->GetParams().Get("gamma")->SetValue(nf::PinValue(1.0f));
    graph.Connect(comp->GetOutputPin("output"), level->GetInputPin("input"));

    // Node 6: Presentation sink
    auto* toWindow = graph.CreateNode<nf::ToWindowTexOp>("WindowSink");
    graph.Connect(level->GetOutputPin("output"), toWindow->GetInputPin("input"));

    // Execute full graph cook
    EXPECT_TRUE(graph.CookAll(ctx));

    // Verify all nodes cooked successfully
    EXPECT_FALSE(bg->IsDirty());
    EXPECT_FALSE(fg->IsDirty());
    EXPECT_FALSE(blur->IsDirty());
    EXPECT_FALSE(comp->IsDirty());
    EXPECT_FALSE(level->IsDirty());
    EXPECT_FALSE(toWindow->IsDirty());

    auto finalTex = toWindow->GetOutputTexture();
    ASSERT_NE(finalTex, nullptr);
    EXPECT_EQ(finalTex->GetWidth(), 1280u);
    EXPECT_EQ(finalTex->GetHeight(), 720u);

    // Read back pixels from final output texture
    std::vector<uint8_t> pixels;
    EXPECT_TRUE(finalTex->ReadbackPixels(pixels));
    EXPECT_EQ(pixels.size(), 1280u * 720u * 4u);

    // Verify pixel math: (Red 1.0 + Blue 1.0) -> Magenta (R > 0, B > 0, G = 0)
    uint8_t r = pixels[0];
    uint8_t g = pixels[1];
    uint8_t b = pixels[2];
    uint8_t a = pixels[3];

    EXPECT_GT(r, 200);
    EXPECT_EQ(g, 0);
    EXPECT_GT(b, 200);
    EXPECT_EQ(a, 255);

    finalTex.reset();
    graph.Clear();
    pool->Clear();
    device->Cleanup();
}
