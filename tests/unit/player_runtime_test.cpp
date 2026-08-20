#include <gtest/gtest.h>
#include "../../src/player/PlayerApp.h"

TEST(PlayerRuntimeTest, CLIArgumentParsing) {
    const char* argv[] = {
        "nodeforge_player.exe",
        "--project", "test_project.nfp",
        "--fullscreen",
        "--fps", "120",
        "--kiosk",
        "--display", "2",
        "--width", "1280",
        "--height", "720",
        "--frames", "500",
        "--benchmark"
    };
    int argc = sizeof(argv) / sizeof(argv[0]);

    nf::player::PlayerOptions opts = nf::player::PlayerApp::ParseArgs(argc, const_cast<char**>(argv));

    EXPECT_EQ(opts.projectPath, "test_project.nfp");
    EXPECT_TRUE(opts.fullscreen);
    EXPECT_TRUE(opts.kioskMode);
    EXPECT_EQ(opts.displayIndex, 2);
    EXPECT_DOUBLE_EQ(opts.targetFps, 120.0);
    EXPECT_EQ(opts.windowWidth, 1280);
    EXPECT_EQ(opts.windowHeight, 720);
    EXPECT_EQ(opts.maxFrames, 500u);
    EXPECT_TRUE(opts.benchmarkMode);
}

TEST(PlayerRuntimeTest, DefaultOptions) {
    const char* argv[] = { "nodeforge_player.exe" };
    int argc = 1;

    nf::player::PlayerOptions opts = nf::player::PlayerApp::ParseArgs(argc, const_cast<char**>(argv));

    EXPECT_EQ(opts.projectPath, "");
    EXPECT_FALSE(opts.fullscreen);
    EXPECT_FALSE(opts.kioskMode);
    EXPECT_EQ(opts.displayIndex, 0);
    EXPECT_DOUBLE_EQ(opts.targetFps, 60.0);
    EXPECT_EQ(opts.windowWidth, 1920);
    EXPECT_EQ(opts.windowHeight, 1080);
    EXPECT_EQ(opts.maxFrames, 0u);
    EXPECT_FALSE(opts.benchmarkMode);
}
