#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "../core/GeometryData.h"

namespace nf {

struct WarpVertex {
    glm::vec2 pos; // Screen position [-1, 1] or [0, 1]
    glm::vec2 uv;  // Source texture coordinate [0, 1]
};

class WarpMesh {
public:
    WarpMesh(int rows = 4, int cols = 4);

    void SetGridSize(int rows, int cols);
    int GetRows() const { return m_rows; }
    int GetCols() const { return m_cols; }

    void Reset();
    void SetControlPoint(int row, int col, const glm::vec2& pos);
    glm::vec2 GetControlPoint(int row, int col) const;

    void SetCornerPin(int cornerIndex, const glm::vec2& pos);
    glm::vec2 GetCornerPin(int cornerIndex) const;

    glm::vec2 Evaluate(float u, float v) const;
    void GenerateGeometry(std::vector<Vertex>& outVertices, std::vector<uint32_t>& outIndices, int subdivRows = 32, int subdivCols = 32) const;

    std::string ToJson() const;
    bool FromJson(const std::string& jsonStr);

private:
    int m_rows = 4;
    int m_cols = 4;
    std::vector<glm::vec2> m_controlPoints; // row * m_cols + col
};

} // namespace nf
