#include "GeometryData.h"
#include <cmath>
#include <numbers>
#include <algorithm>
#include <glm/gtc/noise.hpp>

namespace nf {

GeometryData::GeometryData(std::vector<Vertex> vertices, std::vector<uint32_t> indices)
    : m_vertices(std::move(vertices)), m_indices(std::move(indices)) {
}

void GeometryData::Clear() {
    m_vertices.clear();
    m_indices.clear();
    m_instances.clear();
}

void GeometryData::SetVertices(std::vector<Vertex> vertices) {
    m_vertices = std::move(vertices);
}

void GeometryData::SetIndices(std::vector<uint32_t> indices) {
    m_indices = std::move(indices);
}

void GeometryData::SetInstances(std::vector<InstanceData> instances) {
    m_instances = std::move(instances);
}

GeometryData GeometryData::CreateGrid(float sizeX, float sizeY, int rows, int cols, int plane) {
    rows = std::max(2, rows);
    cols = std::max(2, cols);

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    vertices.reserve(static_cast<size_t>(rows * cols));
    indices.reserve(static_cast<size_t>((rows - 1) * (cols - 1) * 6));

    float halfX = sizeX * 0.5f;
    float halfY = sizeY * 0.5f;

    for (int r = 0; r < rows; ++r) {
        float v = static_cast<float>(r) / static_cast<float>(rows - 1);
        float py = -halfY + v * sizeY;

        for (int c = 0; c < cols; ++c) {
            float u = static_cast<float>(c) / static_cast<float>(cols - 1);
            float px = -halfX + u * sizeX;

            Vertex vert{};
            vert.uv = glm::vec2(u, v);
            vert.color = glm::vec4(1.0f);

            if (plane == 0) { // XY Plane
                vert.pos = glm::vec3(px, py, 0.0f);
                vert.normal = glm::vec3(0.0f, 0.0f, 1.0f);
                vert.tangent = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
            } else if (plane == 1) { // XZ Plane
                vert.pos = glm::vec3(px, 0.0f, py);
                vert.normal = glm::vec3(0.0f, 1.0f, 0.0f);
                vert.tangent = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
            } else { // YZ Plane
                vert.pos = glm::vec3(0.0f, px, py);
                vert.normal = glm::vec3(1.0f, 0.0f, 0.0f);
                vert.tangent = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
            }
            vertices.push_back(vert);
        }
    }

    for (int r = 0; r < rows - 1; ++r) {
        for (int c = 0; c < cols - 1; ++c) {
            uint32_t i0 = static_cast<uint32_t>(r * cols + c);
            uint32_t i1 = static_cast<uint32_t>(r * cols + (c + 1));
            uint32_t i2 = static_cast<uint32_t>((r + 1) * cols + (c + 1));
            uint32_t i3 = static_cast<uint32_t>((r + 1) * cols + c);

            indices.push_back(i0);
            indices.push_back(i1);
            indices.push_back(i2);

            indices.push_back(i0);
            indices.push_back(i2);
            indices.push_back(i3);
        }
    }

    return GeometryData(std::move(vertices), std::move(indices));
}

GeometryData GeometryData::CreateSphere(float radius, int rings, int segments) {
    rings = std::max(3, rings);
    segments = std::max(3, segments);

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    constexpr float pi = std::numbers::pi_v<float>;

    for (int r = 0; r <= rings; ++r) {
        float phi = static_cast<float>(r) * (pi / static_cast<float>(rings));
        float sinPhi = std::sin(phi);
        float cosPhi = std::cos(phi);

        for (int s = 0; s <= segments; ++s) {
            float theta = static_cast<float>(s) * (2.0f * pi / static_cast<float>(segments));
            float sinTheta = std::sin(theta);
            float cosTheta = std::cos(theta);

            glm::vec3 normal(sinPhi * cosTheta, cosPhi, sinPhi * sinTheta);
            glm::vec3 pos = normal * radius;
            glm::vec2 uv(static_cast<float>(s) / segments, static_cast<float>(r) / rings);
            glm::vec4 tangent(-sinTheta, 0.0f, cosTheta, 1.0f);

            vertices.push_back(Vertex{
                .pos = pos,
                .normal = normal,
                .uv = uv,
                .color = glm::vec4(1.0f),
                .tangent = tangent
            });
        }
    }

    for (int r = 0; r < rings; ++r) {
        for (int s = 0; s < segments; ++s) {
            uint32_t current = static_cast<uint32_t>(r * (segments + 1) + s);
            uint32_t next = current + static_cast<uint32_t>(segments + 1);

            indices.push_back(current);
            indices.push_back(next);
            indices.push_back(current + 1);

            indices.push_back(current + 1);
            indices.push_back(next);
            indices.push_back(next + 1);
        }
    }

    return GeometryData(std::move(vertices), std::move(indices));
}

GeometryData GeometryData::CreateBox(float sizeX, float sizeY, float sizeZ, int divsX, int divsY, int divsZ) {
    divsX = std::max(1, divsX);
    divsY = std::max(1, divsY);
    divsZ = std::max(1, divsZ);

    GeometryData box;
    float hx = sizeX * 0.5f;
    float hy = sizeY * 0.5f;
    float hz = sizeZ * 0.5f;

    // Helper to generate a subdivided quad face
    auto addFace = [&](const glm::vec3& origin, const glm::vec3& uVec, const glm::vec3& vVec, const glm::vec3& norm, int uDivs, int vDivs) {
        uint32_t baseIdx = static_cast<uint32_t>(box.GetVertexCount());
        for (int v = 0; v <= vDivs; ++v) {
            float fv = static_cast<float>(v) / vDivs;
            for (int u = 0; u <= uDivs; ++u) {
                float fu = static_cast<float>(u) / uDivs;
                glm::vec3 pos = origin + uVec * fu + vVec * fv;
                box.GetVertices().push_back(Vertex{
                    .pos = pos,
                    .normal = norm,
                    .uv = glm::vec2(fu, fv),
                    .color = glm::vec4(1.0f),
                    .tangent = glm::vec4(glm::normalize(uVec), 1.0f)
                });
            }
        }
        for (int v = 0; v < vDivs; ++v) {
            for (int u = 0; u < uDivs; ++u) {
                uint32_t i0 = baseIdx + static_cast<uint32_t>(v * (uDivs + 1) + u);
                uint32_t i1 = i0 + 1;
                uint32_t i2 = baseIdx + static_cast<uint32_t>((v + 1) * (uDivs + 1) + (u + 1));
                uint32_t i3 = baseIdx + static_cast<uint32_t>((v + 1) * (uDivs + 1) + u);

                box.GetIndices().push_back(i0);
                box.GetIndices().push_back(i1);
                box.GetIndices().push_back(i2);

                box.GetIndices().push_back(i0);
                box.GetIndices().push_back(i2);
                box.GetIndices().push_back(i3);
            }
        }
    };

    // +Z Front
    addFace(glm::vec3(-hx, -hy, hz), glm::vec3(sizeX, 0, 0), glm::vec3(0, sizeY, 0), glm::vec3(0, 0, 1), divsX, divsY);
    // -Z Back
    addFace(glm::vec3(hx, -hy, -hz), glm::vec3(-sizeX, 0, 0), glm::vec3(0, sizeY, 0), glm::vec3(0, 0, -1), divsX, divsY);
    // +X Right
    addFace(glm::vec3(hx, -hy, hz), glm::vec3(0, 0, -sizeZ), glm::vec3(0, sizeY, 0), glm::vec3(1, 0, 0), divsZ, divsY);
    // -X Left
    addFace(glm::vec3(-hx, -hy, -hz), glm::vec3(0, 0, sizeZ), glm::vec3(0, sizeY, 0), glm::vec3(-1, 0, 0), divsZ, divsY);
    // +Y Top
    addFace(glm::vec3(-hx, hy, hz), glm::vec3(sizeX, 0, 0), glm::vec3(0, 0, -sizeZ), glm::vec3(0, 1, 0), divsX, divsZ);
    // -Y Bottom
    addFace(glm::vec3(-hx, -hy, -hz), glm::vec3(sizeX, 0, 0), glm::vec3(0, 0, sizeZ), glm::vec3(0, -1, 0), divsX, divsZ);

    return box;
}

GeometryData GeometryData::CreateTorus(float majorR, float minorR, int majorSegs, int minorSegs) {
    majorSegs = std::max(3, majorSegs);
    minorSegs = std::max(3, minorSegs);

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    constexpr float pi = std::numbers::pi_v<float>;

    for (int i = 0; i <= majorSegs; ++i) {
        float u = static_cast<float>(i) * (2.0f * pi / static_cast<float>(majorSegs));
        float cosU = std::cos(u);
        float sinU = std::sin(u);

        for (int j = 0; j <= minorSegs; ++j) {
            float v = static_cast<float>(j) * (2.0f * pi / static_cast<float>(minorSegs));
            float cosV = std::cos(v);
            float sinV = std::sin(v);

            glm::vec3 pos(
                (majorR + minorR * cosV) * cosU,
                minorR * sinV,
                (majorR + minorR * cosV) * sinU
            );
            glm::vec3 normal(cosV * cosU, sinV, cosV * sinU);
            glm::vec2 uv(static_cast<float>(i) / majorSegs, static_cast<float>(j) / minorSegs);

            vertices.push_back(Vertex{
                .pos = pos,
                .normal = glm::normalize(normal),
                .uv = uv,
                .color = glm::vec4(1.0f),
                .tangent = glm::vec4(-sinU, 0.0f, cosU, 1.0f)
            });
        }
    }

    for (int i = 0; i < majorSegs; ++i) {
        for (int j = 0; j < minorSegs; ++j) {
            uint32_t current = static_cast<uint32_t>(i * (minorSegs + 1) + j);
            uint32_t next = current + static_cast<uint32_t>(minorSegs + 1);

            indices.push_back(current);
            indices.push_back(next);
            indices.push_back(current + 1);

            indices.push_back(current + 1);
            indices.push_back(next);
            indices.push_back(next + 1);
        }
    }

    return GeometryData(std::move(vertices), std::move(indices));
}

GeometryData GeometryData::CreateCylinder(float height, float rBottom, float rTop, int segs, bool capBottom, bool capTop) {
    segs = std::max(3, segs);
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    constexpr float pi = std::numbers::pi_v<float>;
    float halfH = height * 0.5f;

    // Tube side vertices
    for (int ring = 0; ring <= 1; ++ring) {
        float y = (ring == 0) ? -halfH : halfH;
        float r = (ring == 0) ? rBottom : rTop;
        float v = (ring == 0) ? 0.0f : 1.0f;

        for (int s = 0; s <= segs; ++s) {
            float u = static_cast<float>(s) / segs;
            float theta = u * 2.0f * pi;
            float cosT = std::cos(theta);
            float sinT = std::sin(theta);

            glm::vec3 pos(r * cosT, y, r * sinT);
            glm::vec3 normal = glm::normalize(glm::vec3(cosT, (rBottom - rTop) / height, sinT));

            vertices.push_back(Vertex{
                .pos = pos,
                .normal = normal,
                .uv = glm::vec2(u, v),
                .color = glm::vec4(1.0f),
                .tangent = glm::vec4(-sinT, 0.0f, cosT, 1.0f)
            });
        }
    }

    for (int s = 0; s < segs; ++s) {
        uint32_t i0 = static_cast<uint32_t>(s);
        uint32_t i1 = static_cast<uint32_t>(s + segs + 1);
        uint32_t i2 = static_cast<uint32_t>(s + 1);
        uint32_t i3 = static_cast<uint32_t>(s + segs + 2);

        indices.push_back(i0);
        indices.push_back(i1);
        indices.push_back(i2);

        indices.push_back(i2);
        indices.push_back(i1);
        indices.push_back(i3);
    }

    // Top Cap
    if (capTop && rTop > 0.0f) {
        uint32_t centerIdx = static_cast<uint32_t>(vertices.size());
        vertices.push_back(Vertex{ .pos = glm::vec3(0, halfH, 0), .normal = glm::vec3(0, 1, 0), .uv = glm::vec2(0.5f), .color = glm::vec4(1.0f) });
        uint32_t rimStart = static_cast<uint32_t>(vertices.size());

        for (int s = 0; s <= segs; ++s) {
            float theta = (static_cast<float>(s) / segs) * 2.0f * pi;
            vertices.push_back(Vertex{
                .pos = glm::vec3(rTop * std::cos(theta), halfH, rTop * std::sin(theta)),
                .normal = glm::vec3(0, 1, 0),
                .uv = glm::vec2(0.5f + 0.5f * std::cos(theta), 0.5f + 0.5f * std::sin(theta)),
                .color = glm::vec4(1.0f)
            });
        }
        for (int s = 0; s < segs; ++s) {
            indices.push_back(centerIdx);
            indices.push_back(rimStart + static_cast<uint32_t>(s));
            indices.push_back(rimStart + static_cast<uint32_t>(s + 1));
        }
    }

    // Bottom Cap
    if (capBottom && rBottom > 0.0f) {
        uint32_t centerIdx = static_cast<uint32_t>(vertices.size());
        vertices.push_back(Vertex{ .pos = glm::vec3(0, -halfH, 0), .normal = glm::vec3(0, -1, 0), .uv = glm::vec2(0.5f), .color = glm::vec4(1.0f) });
        uint32_t rimStart = static_cast<uint32_t>(vertices.size());

        for (int s = 0; s <= segs; ++s) {
            float theta = (static_cast<float>(s) / segs) * 2.0f * pi;
            vertices.push_back(Vertex{
                .pos = glm::vec3(rBottom * std::cos(theta), -halfH, rBottom * std::sin(theta)),
                .normal = glm::vec3(0, -1, 0),
                .uv = glm::vec2(0.5f + 0.5f * std::cos(theta), 0.5f + 0.5f * std::sin(theta)),
                .color = glm::vec4(1.0f)
            });
        }
        for (int s = 0; s < segs; ++s) {
            indices.push_back(centerIdx);
            indices.push_back(rimStart + static_cast<uint32_t>(s + 1));
            indices.push_back(rimStart + static_cast<uint32_t>(s));
        }
    }

    return GeometryData(std::move(vertices), std::move(indices));
}

void GeometryData::Transform(const glm::mat4& mat) {
    glm::mat3 normMat = glm::transpose(glm::inverse(glm::mat3(mat)));
    for (auto& vert : m_vertices) {
        vert.pos = glm::vec3(mat * glm::vec4(vert.pos, 1.0f));
        vert.normal = glm::normalize(normMat * vert.normal);
        vert.tangent = glm::vec4(glm::normalize(normMat * glm::vec3(vert.tangent)), vert.tangent.w);
    }
}

void GeometryData::Merge(const GeometryData& other) {
    if (other.IsEmpty()) return;

    uint32_t offset = static_cast<uint32_t>(m_vertices.size());
    m_vertices.insert(m_vertices.end(), other.m_vertices.begin(), other.m_vertices.end());

    m_indices.reserve(m_indices.size() + other.m_indices.size());
    for (uint32_t idx : other.m_indices) {
        m_indices.push_back(idx + offset);
    }
}

void GeometryData::ComputeNormals(bool smooth) {
    if (m_vertices.empty() || m_indices.empty()) return;

    if (smooth) {
        for (auto& v : m_vertices) v.normal = glm::vec3(0.0f);

        for (size_t i = 0; i + 2 < m_indices.size(); i += 3) {
            uint32_t i0 = m_indices[i];
            uint32_t i1 = m_indices[i + 1];
            uint32_t i2 = m_indices[i + 2];

            if (i0 < m_vertices.size() && i1 < m_vertices.size() && i2 < m_vertices.size()) {
                glm::vec3 p0 = m_vertices[i0].pos;
                glm::vec3 p1 = m_vertices[i1].pos;
                glm::vec3 p2 = m_vertices[i2].pos;

                glm::vec3 fn = glm::cross(p1 - p0, p2 - p0);
                m_vertices[i0].normal += fn;
                m_vertices[i1].normal += fn;
                m_vertices[i2].normal += fn;
            }
        }

        for (auto& v : m_vertices) {
            float len = glm::length(v.normal);
            if (len > 1e-6f) v.normal /= len;
            else v.normal = glm::vec3(0, 1, 0);
        }
    } else {
        // Flat faceting: duplicate vertices per triangle
        std::vector<Vertex> newVerts;
        std::vector<uint32_t> newIndices;
        newVerts.reserve(m_indices.size());
        newIndices.reserve(m_indices.size());

        for (size_t i = 0; i + 2 < m_indices.size(); i += 3) {
            uint32_t i0 = m_indices[i];
            uint32_t i1 = m_indices[i + 1];
            uint32_t i2 = m_indices[i + 2];

            Vertex v0 = m_vertices[i0];
            Vertex v1 = m_vertices[i1];
            Vertex v2 = m_vertices[i2];

            glm::vec3 fn = glm::normalize(glm::cross(v1.pos - v0.pos, v2.pos - v0.pos));
            v0.normal = fn;
            v1.normal = fn;
            v2.normal = fn;

            uint32_t base = static_cast<uint32_t>(newVerts.size());
            newVerts.push_back(v0);
            newVerts.push_back(v1);
            newVerts.push_back(v2);

            newIndices.push_back(base);
            newIndices.push_back(base + 1);
            newIndices.push_back(base + 2);
        }

        m_vertices = std::move(newVerts);
        m_indices = std::move(newIndices);
    }
}

void GeometryData::ComputeTangents() {
    for (auto& v : m_vertices) v.tangent = glm::vec4(0.0f);

    for (size_t i = 0; i + 2 < m_indices.size(); i += 3) {
        uint32_t i0 = m_indices[i];
        uint32_t i1 = m_indices[i + 1];
        uint32_t i2 = m_indices[i + 2];

        if (i0 < m_vertices.size() && i1 < m_vertices.size() && i2 < m_vertices.size()) {
            const auto& v0 = m_vertices[i0];
            const auto& v1 = m_vertices[i1];
            const auto& v2 = m_vertices[i2];

            glm::vec3 edge1 = v1.pos - v0.pos;
            glm::vec3 edge2 = v2.pos - v0.pos;
            glm::vec2 dUV1 = v1.uv - v0.uv;
            glm::vec2 dUV2 = v2.uv - v0.uv;

            float r = (dUV1.x * dUV2.y - dUV1.y * dUV2.x);
            float f = (std::abs(r) > 1e-6f) ? (1.0f / r) : 1.0f;

            glm::vec3 tangent = (edge1 * dUV2.y - edge2 * dUV1.y) * f;
            m_vertices[i0].tangent += glm::vec4(tangent, 0.0f);
            m_vertices[i1].tangent += glm::vec4(tangent, 0.0f);
            m_vertices[i2].tangent += glm::vec4(tangent, 0.0f);
        }
    }

    for (auto& v : m_vertices) {
        glm::vec3 t = glm::vec3(v.tangent);
        if (glm::length(t) > 1e-6f) {
            t = glm::normalize(t - v.normal * glm::dot(v.normal, t));
            v.tangent = glm::vec4(t, 1.0f);
        } else {
            v.tangent = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        }
    }
}

BoundingBox GeometryData::ComputeBounds() const {
    BoundingBox box{};
    for (const auto& vert : m_vertices) {
        box.min = glm::min(box.min, vert.pos);
        box.max = glm::max(box.max, vert.pos);
    }
    return box;
}

void GeometryData::DeformNoise(float amplitude, float frequency, const glm::vec3& offset, bool alongNormal) {
    for (auto& vert : m_vertices) {
        glm::vec3 samplePos = (vert.pos + offset) * frequency;
        float n = glm::perlin(samplePos);

        if (alongNormal) {
            vert.pos += vert.normal * (n * amplitude);
        } else {
            vert.pos += glm::vec3(n * amplitude);
        }
    }
    ComputeNormals(true);
}

} // namespace nf
