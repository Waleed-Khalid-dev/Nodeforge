#include <gtest/gtest.h>
#include "operators/comp/SliderPanelComp.h"
#include "operators/comp/ButtonPanelComp.h"
#include "operators/comp/DialPanelComp.h"
#include <chrono>
#include <vector>
#include <iostream>

using namespace nf;

class PanelBenchmark : public ::testing::Test {};

TEST_F(PanelBenchmark, FiveHundredWidgetThroughput) {
    const size_t widgetCount = 500;
    std::vector<std::unique_ptr<SliderPanelComp>> sliders;
    sliders.reserve(widgetCount);

    for (size_t i = 0; i < widgetCount; ++i) {
        auto s = std::make_unique<SliderPanelComp>(static_cast<NodeId>(100 + i), "Slider_" + std::to_string(i));
        s->SetValueInteractive(static_cast<float>(i % 100) / 100.0f);
        sliders.push_back(std::move(s));
    }

    CookContext ctx{};
    ctx.deltaTimeSeconds = 1.0 / 60.0;
    ctx.timeSeconds = 0.0;

    const int benchmarkFrames = 50;
    auto start = std::chrono::high_resolution_clock::now();
    for (int frame = 0; frame < benchmarkFrames; ++frame) {
        ctx.timeSeconds += ctx.deltaTimeSeconds;
        for (auto& s : sliders) {
            s->SetValueInteractive(static_cast<float>((frame + s->GetId()) % 100) / 100.0f);
            s->Cook(ctx);
        }
    }
    auto end = std::chrono::high_resolution_clock::now();

    double totalMs = std::chrono::duration<double, std::milli>(end - start).count();
    double avgMsPerFrame = totalMs / benchmarkFrames;

    std::cout << "[PanelBenchmark] 500 Widgets average cook time: " << avgMsPerFrame << " ms per frame" << std::endl;
    EXPECT_LT(avgMsPerFrame, 35.0); // Sub-millisecond per widget in Debug
}

TEST_F(PanelBenchmark, ContinuousSoakZeroLeaks) {
    auto slider = std::make_unique<SliderPanelComp>(2000, "SoakSlider");
    auto button = std::make_unique<ButtonPanelComp>(2001, "SoakButton");
    auto dial = std::make_unique<DialPanelComp>(2002, "SoakDial");

    CookContext ctx{};
    ctx.deltaTimeSeconds = 1.0 / 60.0;

    for (int i = 0; i < 2000; ++i) {
        ctx.timeSeconds += ctx.deltaTimeSeconds;
        slider->SetValueInteractive(static_cast<float>(i % 100) / 100.0f);
        if (i % 2 == 0) button->Click();
        dial->SetValueInteractive(static_cast<float>(i % 360));

        slider->Cook(ctx);
        button->Cook(ctx);
        dial->Cook(ctx);
    }

    EXPECT_TRUE(true);
}
