#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace nf {

struct Vertex {
    glm::vec3 pos{0.0f};
    glm::vec3 normal{0.0f, 1.0f, 0.0f};
    glm::vec2 uv{0.0f};
    glm::vec4 color{1.0f};
    glm::vec4 tangent{1.0f, 0.0f, 0.0f, 1.0f};

    bool operator==(const Vertex& other) const = default;
};

struct BoundingBox {
    glm::vec3 min{ 1e30f };
    glm::vec3 max{-1e30f };

    bool IsValid() const { return min.x <= max.x && min.y <= max.y && min.z <= max.z; }
    glm::vec3 GetCenter() const { return (min + max) * 0.5f; }
    glm::vec3 GetSize() const { return max - min; }
};

struct InstanceData {
    glm::mat4 transform{1.0f};
    glm::vec4 color{1.0f};

    bool operator==(const InstanceData& other) const = default;
};

class GeometryData {
public:
    GeometryData() = default;
    GeometryData(std::vector<Vertex> vertices, std::vector<uint32_t> indices);

    // Queries
    size_t GetVertexCount() const { return m_vertices.size(); }
    size_t GetIndexCount() const { return m_indices.size(); }
    size_t GetTriangleCount() const { return m_indices.size() / 3; }
    size_t GetInstanceCount() const { return m_instances.size(); }
    bool IsEmpty() const { return m_vertices.empty(); }
    void Clear();

    // Data Access
    const std::vector<Vertex>& GetVertices() const { return m_vertices; }
    std::vector<Vertex>& GetVertices() { return m_vertices; }
    const std::vector<uint32_t>& GetIndices() const { return m_indices; }
    std::vector<uint32_t>& GetIndices() { return m_indices; }
    const std::vector<InstanceData>& GetInstances() const { return m_instances; }
    std::vector<InstanceData>& GetInstances() { return m_instances; }

    void SetVertices(std::vector<Vertex> vertices);
    void SetIndices(std::vector<uint32_t> indices);
    void SetInstances(std::vector<InstanceData> instances);

    // Primitive Generators
    static GeometryData CreateGrid(float sizeX = 1.0f, float sizeY = 1.0f, int rows = 10, int cols = 10, int plane = 0);
    static GeometryData CreateSphere(float radius = 1.0f, int rings = 16, int segments = 32);
    static GeometryData CreateBox(float sizeX = 1.0f, float sizeY = 1.0f, float sizeZ = 1.0f, int divsX = 1, int divsY = 1, int divsZ = 1);
    static GeometryData CreateTorus(float majorR = 1.0f, float minorR = 0.3f, int majorSegs = 32, int minorSegs = 16);
    static GeometryData CreateCylinder(float height = 2.0f, float rBottom = 1.0f, float rTop = 1.0f, int segs = 32, bool capBottom = true, bool capTop = true);

    // Modifiers & Mesh Math
    void Transform(const glm::mat4& mat);
    void Merge(const GeometryData& other);
    void ComputeNormals(bool smooth = true);
    void ComputeTangents();
    BoundingBox ComputeBounds() const;
    void DeformNoise(float amplitude = 0.2f, float frequency = 1.0f, const glm::vec3& offset = glm::vec3(0.0f), bool alongNormal = true);

    bool operator==(const GeometryData& other) const = default;

private:
    std::vector<Vertex> m_vertices;
    std::vector<uint32_t> m_indices;
    std::vector<InstanceData> m_instances;
};

} // namespace nf
