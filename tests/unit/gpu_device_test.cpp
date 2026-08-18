#include <gtest/gtest.h>
#include "../../src/gpu/Device.h"
#include <spdlog/spdlog.h>

TEST(GpuDeviceTest, InitializeHeadless) {
    // Setup spdlog to not spam the test output unless it fails
    spdlog::set_level(spdlog::level::err);

    gpu::Device device;
    // Initialize with nullptr for headless mode (no surface)
    bool result = device.Initialize(nullptr);
    
    EXPECT_TRUE(result) << "Device failed to initialize in headless mode";
    
    if (result) {
        EXPECT_NE(device.GetInstance(), VK_NULL_HANDLE);
        EXPECT_NE(device.GetPhysicalDevice(), VK_NULL_HANDLE);
        EXPECT_NE(device.GetDevice(), VK_NULL_HANDLE);
        EXPECT_NE(device.GetAllocator(), VK_NULL_HANDLE);
        EXPECT_NE(device.GetGraphicsQueue(), VK_NULL_HANDLE);
    }

    device.Cleanup();
}
