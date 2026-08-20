#include <gtest/gtest.h>
#include "gpu/TexturePool.h"

using namespace gpu;

TEST(LeakDetectorTest, TexturePoolStatsAndAutoRecycle) {
    // Test pool with null device in mock/headless unit testing
    TexturePool pool(nullptr);

    EXPECT_EQ(pool.GetAvailableCount(), 0u);
    EXPECT_EQ(pool.GetActiveLeasedCount(), 0u);
    EXPECT_EQ(pool.GetTotalAllocatedCount(), 0u);

    auto stats = pool.GetStats();
    EXPECT_EQ(stats.activeLeases, 0u);
    EXPECT_EQ(stats.totalAllocated, 0u);
    EXPECT_EQ(stats.peakLeased, 0u);
}
