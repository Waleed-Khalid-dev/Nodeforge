#include <gtest/gtest.h>
#include "core/GeometryData.h"

using namespace nf;

TEST(GeometryDataTest, PrimitiveGeneratorsAndDimensions) {
    // 1. Grid (10x10 = 100 verts, 9x9x2 = 162 tris)
    GeometryData grid = GeometryData::CreateGrid(2.0f, 2.0f, 10, 10, 0);
    EXPECT_EQ(grid.GetVertexCount(), 100);
    EXPECT_EQ(grid.GetTriangleCount(), 162);
    BoundingBox gridBounds = grid.ComputeBounds();
    EXPECT_NEAR(gridBounds.min.x, -1.0f, 1e-4f);
    EXPECT_NEAR(gridBounds.max.x, 1.0f, 1e-4f);

    // 2. Sphere
    GeometryData sphere = GeometryData::CreateSphere(1.5f, 16, 32);
    EXPECT_GT(sphere.GetVertexCount(), 100);
    EXPECT_GT(sphere.GetTriangleCount(), 200);
    BoundingBox sphBounds = sphere.ComputeBounds();
    EXPECT_NEAR(sphBounds.GetSize().x, 3.0f, 1e-2f);

    // 3. Box (6 faces * 4 verts per quad = 24 verts, 12 tris)
    GeometryData box = GeometryData::CreateBox(1.0f, 2.0f, 3.0f, 1, 1, 1);
    EXPECT_EQ(box.GetVertexCount(), 24);
    EXPECT_EQ(box.GetTriangleCount(), 12);
    BoundingBox boxBounds = box.ComputeBounds();
    EXPECT_NEAR(boxBounds.GetSize().x, 1.0f, 1e-4f);
    EXPECT_NEAR(boxBounds.GetSize().y, 2.0f, 1e-4f);
    EXPECT_NEAR(boxBounds.GetSize().z, 3.0f, 1e-4f);

    // 4. Torus
    GeometryData torus = GeometryData::CreateTorus(2.0f, 0.5f, 16, 8);
    EXPECT_GT(torus.GetVertexCount(), 50);
    EXPECT_GT(torus.GetTriangleCount(), 100);

    // 5. Cylinder
    GeometryData cyl = GeometryData::CreateCylinder(4.0f, 1.0f, 1.0f, 16, true, true);
    EXPECT_GT(cyl.GetVertexCount(), 30);
    EXPECT_GT(cyl.GetTriangleCount(), 30);
}

TEST(GeometryDataTest, TransformAndMerge) {
    GeometryData b1 = GeometryData::CreateBox(1.0f, 1.0f, 1.0f);
    GeometryData b2 = GeometryData::CreateBox(1.0f, 1.0f, 1.0f);

    glm::mat4 tMat = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 0.0f, 0.0f));
    b2.Transform(tMat);

    b1.Merge(b2);
    EXPECT_EQ(b1.GetVertexCount(), 48);
    EXPECT_EQ(b1.GetTriangleCount(), 24);

    BoundingBox mergedBounds = b1.ComputeBounds();
    EXPECT_NEAR(mergedBounds.min.x, -0.5f, 1e-4f);
    EXPECT_NEAR(mergedBounds.max.x, 5.5f, 1e-4f);
}

TEST(GeometryDataTest, NormalsTangentsAndNoiseDeform) {
    GeometryData sphere = GeometryData::CreateSphere(1.0f, 16, 32);
    sphere.ComputeNormals(true);
    sphere.ComputeTangents();

    for (const auto& v : sphere.GetVertices()) {
        EXPECT_NEAR(glm::length(v.normal), 1.0f, 1e-2f);
        EXPECT_GT(glm::length(glm::vec3(v.tangent)), 0.5f);
    }

    // Noise deformation alters bounds
    BoundingBox before = sphere.ComputeBounds();
    sphere.DeformNoise(0.5f, 2.0f, glm::vec3(0.0f), true);
    BoundingBox after = sphere.ComputeBounds();

    EXPECT_NE(before.max.x, after.max.x);
}
