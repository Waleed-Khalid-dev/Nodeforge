#include <gtest/gtest.h>
#include "core/ChannelBuffer.h"
#include "graph/Pin.h"
#include "graph/PinValue.h"
#include "io/input/InputManager.h"
#include "operators/chan/MouseInChanOp.h"
#include "operators/chan/KeyboardInChanOp.h"

using namespace nf;

TEST(InteractiveInputTest, MouseInChanOpSimulationAndChannels) {
    auto& input = InputManager::Instance();
    input.ResetState();

    // Inject mock mouse at (0.35, 0.65) with left click
    input.InjectMockMouse(0.35f, 0.65f, true, false, false, 0.0f, 1.0f);

    MouseInChanOp mouseOp(1, "mouse1");
    mouseOp.SetParam("normalized", true);
    mouseOp.SetParam("active", true);

    CookContext ctx;
    EXPECT_TRUE(mouseOp.Cook(ctx));

    const ChannelBuffer* buf = mouseOp.GetOutputBuffer();
    ASSERT_NE(buf, nullptr);

    EXPECT_EQ(buf->GetChannelCount(), 10u);
    EXPECT_TRUE(buf->HasChannel("x"));
    EXPECT_TRUE(buf->HasChannel("y"));
    EXPECT_TRUE(buf->HasChannel("left"));
    EXPECT_TRUE(buf->HasChannel("wheel_y"));

    EXPECT_FLOAT_EQ(*buf->GetChannelData("x"), 0.35f);
    EXPECT_FLOAT_EQ(*buf->GetChannelData("y"), 0.65f);
    EXPECT_FLOAT_EQ(*buf->GetChannelData("left"), 1.0f);
    EXPECT_FLOAT_EQ(*buf->GetChannelData("right"), 0.0f);
    EXPECT_FLOAT_EQ(*buf->GetChannelData("wheel_y"), 1.0f);
}

TEST(InteractiveInputTest, KeyboardInChanOpKeyTrackingAndPulses) {
    auto& input = InputManager::Instance();
    input.ResetState();

    // Inject space key down and shift down
    input.InjectMockKey("space", true);
    input.InjectMockKey("shift", true);

    KeyboardInChanOp keyOp(2, "keys1");
    keyOp.SetParam("keys", std::string("space enter escape"));
    keyOp.SetParam("include_modifiers", true);
    keyOp.SetParam("include_pulses", true);
    keyOp.SetParam("active", true);

    CookContext ctx;
    EXPECT_TRUE(keyOp.Cook(ctx));

    const ChannelBuffer* buf = keyOp.GetOutputBuffer();
    ASSERT_NE(buf, nullptr);

    EXPECT_TRUE(buf->HasChannel("key_space"));
    EXPECT_TRUE(buf->HasChannel("pulse_space"));
    EXPECT_TRUE(buf->HasChannel("key_enter"));
    EXPECT_TRUE(buf->HasChannel("shift"));
    EXPECT_TRUE(buf->HasChannel("ctrl"));

    EXPECT_FLOAT_EQ(*buf->GetChannelData("key_space"), 1.0f);
    EXPECT_FLOAT_EQ(*buf->GetChannelData("pulse_space"), 1.0f);
    EXPECT_FLOAT_EQ(*buf->GetChannelData("key_enter"), 0.0f);
    EXPECT_FLOAT_EQ(*buf->GetChannelData("shift"), 1.0f);
    EXPECT_FLOAT_EQ(*buf->GetChannelData("ctrl"), 0.0f);
}
