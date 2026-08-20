#include <gtest/gtest.h>
#include "media/VideoDecoder.h"
#include "media/FrameRingBuffer.h"

using namespace nf;

TEST(VideoDecoderTest, RingBufferPushPopAndNearestPTS) {
    FrameRingBuffer ring(4);
    EXPECT_TRUE(ring.IsEmpty());

    VideoFrame f1{ .width = 1920, .height = 1080, .ptsSeconds = 1.0, .frameIndex = 60 };
    VideoFrame f2{ .width = 1920, .height = 1080, .ptsSeconds = 2.0, .frameIndex = 120 };
    VideoFrame f3{ .width = 1920, .height = 1080, .ptsSeconds = 3.0, .frameIndex = 180 };

    ring.Push(std::move(f1));
    ring.Push(std::move(f2));
    ring.Push(std::move(f3));

    EXPECT_EQ(ring.Size(), 3);

    VideoFrame nearest;
    EXPECT_TRUE(ring.GetNearestFrame(1.9, nearest));
    EXPECT_EQ(nearest.frameIndex, 120);

    VideoFrame oldest;
    EXPECT_TRUE(ring.PopOldest(oldest));
    EXPECT_EQ(oldest.frameIndex, 60);
    EXPECT_EQ(ring.Size(), 2);
}

TEST(VideoDecoderTest, PlaybackStateSpeedAndSeek) {
    VideoDecoder decoder;
    EXPECT_TRUE(decoder.OpenFile("sample_test.mp4"));

    const auto& info = decoder.GetInfo();
    EXPECT_TRUE(info.isLoaded);
    EXPECT_EQ(info.width, 1920);
    EXPECT_EQ(info.height, 1080);
    EXPECT_DOUBLE_EQ(info.durationSeconds, 10.0);

    decoder.Play();
    EXPECT_TRUE(decoder.IsPlaying());

    decoder.Pause();
    EXPECT_FALSE(decoder.IsPlaying());

    decoder.SetSpeed(2.0f);
    decoder.SetLoopMode(VideoLoopMode::PingPong);

    decoder.Seek(5.0);
    EXPECT_NEAR(decoder.GetCurrentTime(), 5.0, 1e-4);

    VideoFrame frame;
    EXPECT_TRUE(decoder.GetFrame(5.0, frame));
    EXPECT_FALSE(frame.rgbaPixels.empty());
}
