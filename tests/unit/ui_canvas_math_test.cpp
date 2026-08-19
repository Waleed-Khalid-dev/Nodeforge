#include <gtest/gtest.h>
#include "../../src/ui/EditorContext.h"
#include "../../src/ui/canvas/NodeCanvas.h"

using namespace nf;
using namespace nf::ui;

TEST(UICanvasMathTest, CoordinateTransformsRoundTrip) {
    EditorContext ctx;
    NodeCanvas canvas(&ctx);

    ctx.GetCanvasPan() = glm::vec2(150.0f, -80.0f);
    ctx.GetCanvasZoom() = 1.5f;

    glm::vec2 originalCanvasPos(240.0f, 310.0f);
    ImVec2 screenPos = canvas.CanvasToScreen(originalCanvasPos);
    glm::vec2 roundTripPos = canvas.ScreenToCanvas(screenPos);

    EXPECT_NEAR(originalCanvasPos.x, roundTripPos.x, 0.001f);
    EXPECT_NEAR(originalCanvasPos.y, roundTripPos.y, 0.001f);
}

TEST(UICanvasMathTest, ZoomScalingMath) {
    EditorContext ctx;
    NodeCanvas canvas(&ctx);

    ctx.GetCanvasPan() = glm::vec2(0.0f, 0.0f);
    ctx.GetCanvasZoom() = 2.0f;

    glm::vec2 canvasPos(100.0f, 100.0f);
    ImVec2 screenPos = canvas.CanvasToScreen(canvasPos);

    // With pan=(0,0) and zoom=2.0, screen = canvas * 2.0 (relative to canvasOrigin)
    glm::vec2 mappedBack = canvas.ScreenToCanvas(screenPos);
    EXPECT_NEAR(mappedBack.x, 100.0f, 0.001f);
    EXPECT_NEAR(mappedBack.y, 100.0f, 0.001f);
}
