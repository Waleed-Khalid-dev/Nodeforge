#include "WarpMesh.h"
#include <nlohmann/json.hpp>
#include <algorithm>

namespace nf {

WarpMesh::WarpMesh(int rows, int cols) {
    SetGridSize(rows, cols);
}

void WarpMesh::SetGridSize(int rows, int cols) {
    m_rows = std::clamp(rows, 2, 32);
    m_cols = std::clamp(cols, 2, 32);
    Reset();
}

void WarpMesh::Reset() {
    m_controlPoints.resize(m_rows * m_cols);
    for (int r = 0; r < m_rows; ++r) {
        for (int c = 0; c < m_cols; ++c) {
            float u = (m_cols > 1) ? static_cast<float>(c) / (m_cols - 1) : 0.0f;
            float v = (m_rows > 1) ? static_cast<float>(r) / (m_rows - 1) : 0.0f;
            m_controlPoints[r * m_cols + c] = glm::vec2(u, v);
        }
    }
}

void WarpMesh::SetControlPoint(int row, int col, const glm::vec2& pos) {
    if (row >= 0 && row < m_rows && col >= 0 && col < m_cols) {
        m_controlPoints[row * m_cols + col] = pos;
    }
}

glm::vec2 WarpMesh::GetControlPoint(int row, int col) const {
    if (row >= 0 && row < m_rows && col >= 0 && col < m_cols) {
        return m_controlPoints[row * m_cols + col];
    }
    return glm::vec2(0.0f);
}

void WarpMesh::SetCornerPin(int cornerIndex, const glm::vec2& pos) {
    switch (cornerIndex) {
        case 0: SetControlPoint(0, 0, pos); break;                          // Top-Left
        case 1: SetControlPoint(0, m_cols - 1, pos); break;                 // Top-Right
        case 2: SetControlPoint(m_rows - 1, m_cols - 1, pos); break;         // Bottom-Right
        case 3: SetControlPoint(m_rows - 1, 0, pos); break;                 // Bottom-Left
        default: break;
    }
}

glm::vec2 WarpMesh::GetCornerPin(int cornerIndex) const {
    switch (cornerIndex) {
        case 0: return GetControlPoint(0, 0);
        case 1: return GetControlPoint(0, m_cols - 1);
        case 2: return GetControlPoint(m_rows - 1, m_cols - 1);
        case 3: return GetControlPoint(m_rows - 1, 0);
        default: return glm::vec2(0.0f);
    }
}

glm::vec2 WarpMesh::Evaluate(float u, float v) const {
    u = std::clamp(u, 0.0f, 1.0f);
    v = std::clamp(v, 0.0f, 1.0f);

    float cF = u * (m_cols - 1);
    float rF = v * (m_rows - 1);

    int c0 = static_cast<int>(cF);
    int r0 = static_cast<int>(rF);
    int c1 = std::min(c0 + 1, m_cols - 1);
    int r1 = std::min(r0 + 1, m_rows - 1);

    float fracX = cF - c0;
    float fracY = rF - r0;

    glm::vec2 p00 = GetControlPoint(r0, c0);
    glm::vec2 p10 = GetControlPoint(r0, c1);
    glm::vec2 p01 = GetControlPoint(r1, c0);
    glm::vec2 p11 = GetControlPoint(r1, c1);

    glm::vec2 top = glm::mix(p00, p10, fracX);
    glm::vec2 bottom = glm::mix(p01, p11, fracX);

    return glm::mix(top, bottom, fracY);
}

void WarpMesh::GenerateGeometry(std::vector<Vertex>& outVertices, std::vector<uint32_t>& outIndices, int subdivRows, int subdivCols) const {
    subdivRows = std::max(2, subdivRows);
    subdivCols = std::max(2, subdivCols);

    outVertices.clear();
    outIndices.clear();

    outVertices.reserve((subdivRows + 1) * (subdivCols + 1));
    outIndices.reserve(subdivRows * subdivCols * 6);

    for (int r = 0; r <= subdivRows; ++r) {
        float v = static_cast<float>(r) / subdivRows;
        for (int c = 0; c <= subdivCols; ++c) {
            float u = static_cast<float>(c) / subdivCols;

            glm::vec2 warped = Evaluate(u, v);

            // Map warped [0, 1] to screen NDC [-1, 1]
            float screenX = warped.x * 2.0f - 1.0f;
            float screenY = (1.0f - warped.y) * 2.0f - 1.0f;

            outVertices.push_back(Vertex{
                .pos = glm::vec3(screenX, screenY, 0.0f),
                .normal = glm::vec3(0, 0, 1),
                .uv = glm::vec2(u, v),
                .color = glm::vec4(1.0f),
                .tangent = glm::vec4(1, 0, 0, 1)
            });
        }
    }

    for (int r = 0; r < subdivRows; ++r) {
        for (int c = 0; c < subdivCols; ++c) {
            uint32_t i00 = static_cast<uint32_t>(r * (subdivCols + 1) + c);
            uint32_t i10 = static_cast<uint32_t>(r * (subdivCols + 1) + (c + 1));
            uint32_t i01 = static_cast<uint32_t>((r + 1) * (subdivCols + 1) + c);
            uint32_t i11 = static_cast<uint32_t>((r + 1) * (subdivCols + 1) + (c + 1));

            // Triangle 1
            outIndices.push_back(i00);
            outIndices.push_back(i10);
            outIndices.push_back(i01);

            // Triangle 2
            outIndices.push_back(i10);
            outIndices.push_back(i11);
            outIndices.push_back(i01);
        }
    }
}

std::string WarpMesh::ToJson() const {
    nlohmann::json j;
    j["rows"] = m_rows;
    j["cols"] = m_cols;
    auto ptsArr = nlohmann::json::array();
    for (const auto& p : m_controlPoints) {
        ptsArr.push_back({ p.x, p.y });
    }
    j["points"] = ptsArr;
    return j.dump();
}

bool WarpMesh::FromJson(const std::string& jsonStr) {
    try {
        auto j = nlohmann::json::parse(jsonStr);
        m_rows = j.value("rows", 4);
        m_cols = j.value("cols", 4);
        m_controlPoints.clear();
        if (j.contains("points") && j["points"].is_array()) {
            for (const auto& p : j["points"]) {
                if (p.is_array() && p.size() >= 2) {
                    m_controlPoints.push_back(glm::vec2(p[0].get<float>(), p[1].get<float>()));
                }
            }
        }
        if (m_controlPoints.size() != static_cast<size_t>(m_rows * m_cols)) {
            Reset();
        }
        return true;
    } catch (...) {
        Reset();
        return false;
    }
}

} // namespace nf
