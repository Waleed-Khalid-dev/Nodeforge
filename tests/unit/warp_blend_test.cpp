#include <gtest/gtest.h>
#include "media/WarpMesh.h"

using namespace nf;

TEST(WarpMeshTest, GridInitializationAndControlPoints) {
    WarpMesh mesh(4, 4);
    EXPECT_EQ(mesh.GetRows(), 4);
    EXPECT_EQ(mesh.GetCols(), 4);

    // Corner pin 0 (Top-Left) should start at (0, 0)
    EXPECT_EQ(mesh.GetCornerPin(0), glm::vec2(0.0f, 0.0f));
    // Corner pin 2 (Bottom-Right) should start at (1, 1)
    EXPECT_EQ(mesh.GetCornerPin(2), glm::vec2(1.0f, 1.0f));

    // Update corner pin
    mesh.SetCornerPin(0, glm::vec2(0.1f, 0.2f));
    EXPECT_EQ(mesh.GetCornerPin(0), glm::vec2(0.1f, 0.2f));

    // Bilinear evaluation
    glm::vec2 mid = mesh.Evaluate(0.5f, 0.5f);
    EXPECT_GT(mid.x, 0.0f);
    EXPECT_GT(mid.y, 0.0f);
}

TEST(WarpMeshTest, GeometryGeneration) {
    WarpMesh mesh(4, 4);
    std::vector<Vertex> verts;
    std::vector<uint32_t> indices;

    mesh.GenerateGeometry(verts, indices, 16, 16);

    EXPECT_EQ(verts.size(), (16 + 1) * (16 + 1));
    EXPECT_EQ(indices.size(), 16 * 16 * 6);
}

TEST(WarpMeshTest, JsonSerializationRoundtrip) {
    WarpMesh m1(3, 3);
    m1.SetControlPoint(1, 1, glm::vec2(0.6f, 0.7f));

    std::string jsonStr = m1.ToJson();
    EXPECT_FALSE(jsonStr.empty());

    WarpMesh m2;
    EXPECT_TRUE(m2.FromJson(jsonStr));
    EXPECT_EQ(m2.GetRows(), 3);
    EXPECT_EQ(m2.GetCols(), 3);
    EXPECT_NEAR(m2.GetControlPoint(1, 1).x, 0.6f, 1e-4f);
    EXPECT_NEAR(m2.GetControlPoint(1, 1).y, 0.7f, 1e-4f);
}
