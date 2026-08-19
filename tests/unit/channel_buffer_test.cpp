#include <gtest/gtest.h>
#include "core/ChannelBuffer.h"
#include <cmath>

using namespace nf;

TEST(ChannelBufferTest, ConstructionAndDimensions) {
    std::vector<std::string> names = { "tx", "ty", "tz" };
    ChannelBuffer buf(names, 100, 60.0f, 0);

    EXPECT_EQ(buf.GetChannelCount(), 3);
    EXPECT_EQ(buf.GetSampleCount(), 100);
    EXPECT_EQ(buf.GetTotalElements(), 300);
    EXPECT_FLOAT_EQ(buf.GetSampleRate(), 60.0f);
    EXPECT_FALSE(buf.IsEmpty());

    EXPECT_EQ(buf.FindChannelIndex("tx"), 0);
    EXPECT_EQ(buf.FindChannelIndex("ty"), 1);
    EXPECT_EQ(buf.FindChannelIndex("tz"), 2);
    EXPECT_EQ(buf.FindChannelIndex("nonexistent"), -1);
}

TEST(ChannelBufferTest, SampleReadWriteAndSpan) {
    ChannelBuffer buf;
    buf.AddChannel("chan1", std::vector<float>{ 1.0f, 2.0f, 3.0f, 4.0f });

    EXPECT_EQ(buf.GetChannelCount(), 1);
    EXPECT_EQ(buf.GetSampleCount(), 4);

    EXPECT_FLOAT_EQ(buf.GetSample(0, 0), 1.0f);
    EXPECT_FLOAT_EQ(buf.GetSample(0, 1), 2.0f);
    EXPECT_FLOAT_EQ(buf.GetSample("chan1", 2), 3.0f);
    EXPECT_FLOAT_EQ(buf.GetSample("chan1", 3), 4.0f);

    buf.SetSample("chan1", 1, 42.0f);
    EXPECT_FLOAT_EQ(buf.GetSample(0, 1), 42.0f);

    auto span = buf.GetChannelSpan(0);
    EXPECT_EQ(span.size(), 4);
    EXPECT_FLOAT_EQ(span[1], 42.0f);
}

TEST(ChannelBufferTest, SIMDMathOperations) {
    std::vector<std::string> names = { "a", "b" };
    ChannelBuffer buf(names, 4, 60.0f);

    float* a = buf.GetChannelData(0);
    float* b = buf.GetChannelData(1);
    a[0] = 1.0f; a[1] = 2.0f; a[2] = 3.0f; a[3] = 4.0f;
    b[0] = 10.0f; b[1] = 20.0f; b[2] = 30.0f; b[3] = 40.0f;

    // Add scalar
    buf.Add(5.0f);
    EXPECT_FLOAT_EQ(buf.GetSample(0, 0), 6.0f);
    EXPECT_FLOAT_EQ(buf.GetSample(1, 0), 15.0f);

    // Multiply scalar
    buf.Multiply(2.0f);
    EXPECT_FLOAT_EQ(buf.GetSample(0, 0), 12.0f);
    EXPECT_FLOAT_EQ(buf.GetSample(1, 0), 30.0f);

    // Remap
    buf.Remap(0.0f, 100.0f, 0.0f, 1.0f);
    EXPECT_NEAR(buf.GetSample(0, 0), 0.12f, 1e-4f);
    EXPECT_NEAR(buf.GetSample(1, 0), 0.30f, 1e-4f);

    // Clamp
    buf.Clamp(0.2f, 0.5f);
    EXPECT_FLOAT_EQ(buf.GetSample(0, 0), 0.2f);
    EXPECT_FLOAT_EQ(buf.GetSample(1, 0), 0.3f);
}

TEST(ChannelBufferTest, SliceAndResample) {
    ChannelBuffer buf({ "sine" }, 10, 60.0f);
    float* data = buf.GetChannelData(0);
    for (size_t i = 0; i < 10; ++i) {
        data[i] = static_cast<float>(i);
    }

    // Slice
    ChannelBuffer slice = buf.Slice(3, 4);
    EXPECT_EQ(slice.GetSampleCount(), 4);
    EXPECT_FLOAT_EQ(slice.GetSample(0, 0), 3.0f);
    EXPECT_FLOAT_EQ(slice.GetSample(0, 3), 6.0f);

    // Resample
    ChannelBuffer resampled = buf.Resample(19);
    EXPECT_EQ(resampled.GetSampleCount(), 19);
    EXPECT_FLOAT_EQ(resampled.GetSample(0, 0), 0.0f);
    EXPECT_FLOAT_EQ(resampled.GetSample(0, 18), 9.0f);
    EXPECT_NEAR(resampled.GetSample(0, 9), 4.5f, 1e-4f);
}
