#include <gtest/gtest.h>
#include "graph/Graph.h"
#include "gpu/Device.h"
#include "gpu/TexturePool.h"
#include "operators/tex/ConstantTexOp.h"
#include "operators/tex/BlurTexOp.h"
#include "operators/tex/CompositeTexOp.h"
#include "operators/tex/LevelTexOp.h"
#include "operators/tex/TransformTexOp.h"
#include <chrono>

TEST(TexOpBenchmarkTest, TexturePoolZeroMemoryLeakAcross10000Frames) {
    auto device = std::make_unique<gpu::Device>();
    ASSERT_TRUE(device->Initialize(nullptr));

    auto pool = std::make_unique<gpu::TexturePool>(device.get());

    // Run 10,000 frames acquiring and releasing textures
    for (uint64_t frame = 1; frame <= 10000; ++frame) {
        {
            auto tex1 = pool->Acquire(1920, 1080, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, frame);
            auto tex2 = pool->Acquire(1920, 1080, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, frame);
            auto tex3 = pool->Acquire(1280, 720, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, frame);
            EXPECT_NE(tex1, nullptr);
            EXPECT_NE(tex2, nullptr);
            EXPECT_NE(tex3, nullptr);
        }
        // At end of scope, textures return to available pool
    }

    // After 10,000 frames, only exactly 3 textures should have ever been allocated
    EXPECT_EQ(pool->GetTotalAllocatedCount(), 3u);
    EXPECT_EQ(pool->GetAvailableCount(), 3u);

    pool->Clear();
    device->Cleanup();
}

TEST(TexOpBenchmarkTest, FiveNodeChainUnder4ms) {
    auto device = std::make_unique<gpu::Device>();
    ASSERT_TRUE(device->Initialize(nullptr));

    auto pool = std::make_unique<gpu::TexturePool>(device.get());

    nf::CookContext ctx{};
    ctx.frameIndex = 1;
    ctx.timeSeconds = 0.0;
    ctx.gpuDevice = device.get();
    ctx.texturePool = pool.get();

    nf::Graph graph;
    auto* op1 = graph.CreateNode<nf::ConstantTexOp>("N1");
    auto* op2 = graph.CreateNode<nf::TransformTexOp>("N2");
    auto* op3 = graph.CreateNode<nf::BlurTexOp>("N3");
    auto* op4 = graph.CreateNode<nf::LevelTexOp>("N4");
    auto* op5 = graph.CreateNode<nf::CompositeTexOp>("N5");

    graph.Connect(op1->GetOutputPin("output"), op2->GetInputPin("input"));
    graph.Connect(op2->GetOutputPin("output"), op3->GetInputPin("input"));
    graph.Connect(op3->GetOutputPin("output"), op4->GetInputPin("input"));
    graph.Connect(op4->GetOutputPin("output"), op5->GetInputPin("input1"));
    graph.Connect(op1->GetOutputPin("output"), op5->GetInputPin("input2"));

    // Warm-up
    EXPECT_TRUE(graph.CookAll(ctx));

    // Measure 100 continuous cook iterations
    auto start = std::chrono::high_resolution_clock::now();
    const int iterations = 100;
    for (int i = 0; i < iterations; ++i) {
        ctx.frameIndex++;
        op1->MarkDirty(); // Mark root dirty to invalidate full pipeline
        graph.CookAll(ctx);
    }
    auto end = std::chrono::high_resolution_clock::now();

    double totalMs = std::chrono::duration<double, std::milli>(end - start).count();
    double avgMs = totalMs / iterations;

    // Benchmark budget: 5-node chain average cook time
#ifdef NF_DEBUG
    EXPECT_LT(avgMs, 25.0);
#else
    EXPECT_LT(avgMs, 4.0);
#endif

    graph.Clear();
    pool->Clear();
    device->Cleanup();
}
