#include "InstanceGeomOp.h"
#include "../../core/DataTable.h"
#include "../../core/ChannelBuffer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <cmath>
#include <algorithm>

namespace nf {

static glm::mat4 RotationFromTo(const glm::vec3& from, const glm::vec3& to) {
    glm::vec3 v1 = glm::normalize(from);
    glm::vec3 v2 = glm::normalize(to);
    float cosTheta = glm::dot(v1, v2);

    if (cosTheta >= 0.9999f) {
        return glm::mat4(1.0f);
    }
    if (cosTheta <= -0.9999f) {
        // 180 degree flip around perpendicular axis
        glm::vec3 axis = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), v1);
        if (glm::length(axis) < 0.01f) {
            axis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), v1);
        }
        axis = glm::normalize(axis);
        return glm::rotate(glm::mat4(1.0f), 3.14159265f, axis);
    }

    glm::vec3 axis = glm::normalize(glm::cross(v1, v2));
    float angle = std::acos(std::clamp(cosTheta, -1.0f, 1.0f));
    return glm::rotate(glm::mat4(1.0f), angle, axis);
}

InstanceGeomOp::InstanceGeomOp(NodeId id, const std::string& name)
    : GeomOp(id, name, "InstanceGeomOp") {
    m_inTemplatePin = AddInputPin("template_mesh", PinType::Geom);
    m_inDistMeshPin = AddInputPin("distribution_mesh", PinType::Geom);
    m_inDataPin = AddInputPin("instances_data", PinType::Data);
    m_inChanPin = AddInputPin("instances_chan", PinType::Chan);
    m_outPin = AddOutputPin("output", PinType::Geom);

    SetParam("distribution_mode", static_cast<int32_t>(2)); // 0: Table/Chan, 1: Mesh, 2: Grid, 3: Spiral
    SetParam("grid_count", glm::vec3(30.0f, 30.0f, 1.0f));
    SetParam("grid_spacing", glm::vec3(1.0f, 1.0f, 1.0f));
    SetParam("grid_center", true);
    SetParam("spiral_count", static_cast<int32_t>(5000));
    SetParam("spiral_radius_scale", 0.5f);
    SetParam("spiral_twist", 137.5077f);
    SetParam("noise_displacement", 0.0f);
    SetParam("noise_frequency", 0.5f);
    SetParam("align_to_normals", true);
    SetParam("color_mode", static_cast<int32_t>(1)); // 0: Uniform, 1: Index Ramp, 2: Distance Ramp
    SetParam("start_color", glm::vec4(0.1f, 0.7f, 1.0f, 1.0f));
    SetParam("end_color", glm::vec4(1.0f, 0.2f, 0.6f, 1.0f));
    SetParam("instance_scale", glm::vec3(1.0f, 1.0f, 1.0f));
    SetParam("audio_scale_mod", 1.0f);
}

bool InstanceGeomOp::Cook(const CookContext& context) {
    const GeometryData* templateGeom = GetInputGeometry(0);
    GeometryData outGeom;

    if (templateGeom && !templateGeom->IsEmpty()) {
        outGeom = *templateGeom;
    } else {
        // Default template: Simple small cube
        std::vector<Vertex> verts = {
            {{-0.1f, -0.1f,  0.1f}, {0, 0, 1}, {0, 0}, {1, 1, 1, 1}, {1, 0, 0, 1}},
            {{ 0.1f, -0.1f,  0.1f}, {0, 0, 1}, {1, 0}, {1, 1, 1, 1}, {1, 0, 0, 1}},
            {{ 0.1f,  0.1f,  0.1f}, {0, 0, 1}, {1, 1}, {1, 1, 1, 1}, {1, 0, 0, 1}},
            {{-0.1f,  0.1f,  0.1f}, {0, 0, 1}, {0, 1}, {1, 1, 1, 1}, {1, 0, 0, 1}}
        };
        std::vector<uint32_t> indices = {0, 1, 2, 0, 2, 3};
        outGeom = GeometryData(std::move(verts), std::move(indices));
    }

    int32_t mode = GetParam("distribution_mode").Is<int32_t>() ? GetParam("distribution_mode").Get<int32_t>() : 2;
    int32_t colorMode = GetParam("color_mode").Is<int32_t>() ? GetParam("color_mode").Get<int32_t>() : 1;
    glm::vec4 startColor = GetParam("start_color").Is<glm::vec4>() ? GetParam("start_color").Get<glm::vec4>() : glm::vec4(0.1f, 0.7f, 1.0f, 1.0f);
    glm::vec4 endColor = GetParam("end_color").Is<glm::vec4>() ? GetParam("end_color").Get<glm::vec4>() : glm::vec4(1.0f, 0.2f, 0.6f, 1.0f);
    glm::vec3 baseScale = GetParam("instance_scale").Is<glm::vec3>() ? GetParam("instance_scale").Get<glm::vec3>() : glm::vec3(1.0f);
    float noiseDisp = GetParam("noise_displacement").Is<float>() ? GetParam("noise_displacement").Get<float>() : 0.0f;
    float noiseFreq = GetParam("noise_frequency").Is<float>() ? GetParam("noise_frequency").Get<float>() : 0.5f;
    float time = static_cast<float>(context.timeSeconds);

    std::vector<InstanceData> instances;

    // Optional audio modulation buffer
    const ChannelBuffer* chanBuf = nullptr;
    if (m_inChanPin && m_inChanPin->GetValue().Is<ChannelBuffer>()) {
        chanBuf = &m_inChanPin->GetValue().Get<ChannelBuffer>();
    }

    if (mode == 0) {
        // Table or Channel Mode
        if (m_inDataPin && m_inDataPin->GetValue().Is<DataTable>()) {
            const auto& table = m_inDataPin->GetValue().Get<DataTable>();
            size_t rowCount = table.GetRowCount();
            instances.reserve(rowCount);

            for (size_t r = 0; r < rowCount; ++r) {
                glm::vec3 pos(0.0f);
                glm::vec3 scl = baseScale;
                glm::vec4 col = startColor;

                if (table.GetColumnCount() >= 3) {
                    pos.x = std::strtof(table.GetCell(r, 0).c_str(), nullptr);
                    pos.y = std::strtof(table.GetCell(r, 1).c_str(), nullptr);
                    pos.z = std::strtof(table.GetCell(r, 2).c_str(), nullptr);
                }

                glm::mat4 m(1.0f);
                m[0][0] = scl.x;
                m[1][1] = scl.y;
                m[2][2] = scl.z;
                m[3] = glm::vec4(pos, 1.0f);

                InstanceData inst{};
                inst.transform = m;
                inst.color = col;
                instances.push_back(inst);
            }
        } else if (chanBuf && chanBuf->GetSampleCount() > 0) {
            size_t sampleCount = chanBuf->GetSampleCount();
            instances.reserve(sampleCount);

            for (size_t s = 0; s < sampleCount; ++s) {
                glm::vec3 pos(0.0f);
                if (chanBuf->GetChannelCount() >= 3) {
                    pos.x = chanBuf->GetChannelData(0)[s];
                    pos.y = chanBuf->GetChannelData(1)[s];
                    pos.z = chanBuf->GetChannelData(2)[s];
                } else if (chanBuf->GetChannelCount() >= 1) {
                    pos.x = static_cast<float>(s) * 0.5f;
                    pos.y = chanBuf->GetChannelData(0)[s];
                }

                glm::mat4 m(1.0f);
                m[0][0] = baseScale.x;
                m[1][1] = baseScale.y;
                m[2][2] = baseScale.z;
                m[3] = glm::vec4(pos, 1.0f);

                InstanceData inst{};
                inst.transform = m;
                float progress = sampleCount > 1 ? static_cast<float>(s) / static_cast<float>(sampleCount - 1) : 0.0f;
                inst.color = glm::mix(startColor, endColor, progress);
                instances.push_back(inst);
            }
        }
    } else if (mode == 1) {
        // Mesh Surface Mode
        const GeometryData* distGeom = GetInputGeometry(1);
        if (distGeom && !distGeom->IsEmpty()) {
            const auto& vertices = distGeom->GetVertices();
            instances.reserve(vertices.size());
            bool alignNormals = GetParam("align_to_normals").Is<bool>() ? GetParam("align_to_normals").Get<bool>() : true;

            for (size_t i = 0; i < vertices.size(); ++i) {
                const auto& v = vertices[i];
                glm::mat4 m(1.0f);

                if (alignNormals && glm::length(v.normal) > 0.01f) {
                    m = glm::translate(glm::mat4(1.0f), v.pos) * RotationFromTo(glm::vec3(0.0f, 1.0f, 0.0f), v.normal) * glm::scale(glm::mat4(1.0f), baseScale);
                } else {
                    m[0][0] = baseScale.x;
                    m[1][1] = baseScale.y;
                    m[2][2] = baseScale.z;
                    m[3] = glm::vec4(v.pos, 1.0f);
                }

                InstanceData inst{};
                inst.transform = m;
                float progress = vertices.size() > 1 ? static_cast<float>(i) / static_cast<float>(vertices.size() - 1) : 0.0f;
                inst.color = (colorMode == 1) ? glm::mix(startColor, endColor, progress) : startColor;
                instances.push_back(inst);
            }
        }
    } else if (mode == 2) {
        // Parametric Grid Array Mode
        glm::vec3 countV = GetParam("grid_count").Is<glm::vec3>() ? GetParam("grid_count").Get<glm::vec3>() : glm::vec3(30.0f, 30.0f, 1.0f);
        glm::ivec3 count = glm::max(glm::ivec3(1), glm::ivec3(countV));
        glm::vec3 spacing = GetParam("grid_spacing").Is<glm::vec3>() ? GetParam("grid_spacing").Get<glm::vec3>() : glm::vec3(1.0f);
        bool center = GetParam("grid_center").Is<bool>() ? GetParam("grid_center").Get<bool>() : true;

        size_t total = static_cast<size_t>(count.x * count.y * count.z);
        instances.reserve(total);

        glm::vec3 offset(0.0f);
        if (center) {
            offset = -glm::vec3(count - glm::ivec3(1)) * spacing * 0.5f;
        }

        float maxDist = glm::length(glm::vec3(count) * spacing * 0.5f);
        size_t idx = 0;

        for (int z = 0; z < count.z; ++z) {
            for (int y = 0; y < count.y; ++y) {
                for (int x = 0; x < count.x; ++x) {
                    glm::vec3 pos = offset + glm::vec3(x, y, z) * spacing;

                    // Audio ripple displacement
                    if (chanBuf && chanBuf->GetSampleCount() > 0) {
                        size_t sampleIdx = idx % chanBuf->GetSampleCount();
                        float aud = chanBuf->GetChannelData(0)[sampleIdx];
                        pos.y += aud * GetParam("audio_scale_mod").Get<float>();
                    }

                    // Noise jitter
                    if (noiseDisp > 0.0f) {
                        float n = std::sin(pos.x * noiseFreq + time) * std::cos(pos.z * noiseFreq + time);
                        pos.y += n * noiseDisp;
                    }

                    glm::mat4 m(1.0f);
                    m[0][0] = baseScale.x;
                    m[1][1] = baseScale.y;
                    m[2][2] = baseScale.z;
                    m[3] = glm::vec4(pos, 1.0f);

                    InstanceData inst{};
                    inst.transform = m;

                    if (colorMode == 1) {
                        float prog = total > 1 ? static_cast<float>(idx) / static_cast<float>(total - 1) : 0.0f;
                        inst.color = glm::mix(startColor, endColor, prog);
                    } else if (colorMode == 2) {
                        float d = glm::length(pos);
                        float prog = maxDist > 0.001f ? std::clamp(d / maxDist, 0.0f, 1.0f) : 0.0f;
                        inst.color = glm::mix(startColor, endColor, prog);
                    } else {
                        inst.color = startColor;
                    }

                    instances.push_back(inst);
                    idx++;
                }
            }
        }
    } else if (mode == 3) {
        // Fibonacci Phyllotaxis Spiral Mode
        int32_t count = GetParam("spiral_count").Is<int32_t>() ? GetParam("spiral_count").Get<int32_t>() : 5000;
        count = std::max(1, count);
        float radiusScale = GetParam("spiral_radius_scale").Is<float>() ? GetParam("spiral_radius_scale").Get<float>() : 0.5f;
        float twist = GetParam("spiral_twist").Is<float>() ? GetParam("spiral_twist").Get<float>() : 137.5077f;
        float twistRad = glm::radians(twist);

        instances.reserve(count);
        float maxR = radiusScale * std::sqrt(static_cast<float>(count));

        for (int32_t i = 0; i < count; ++i) {
            float fi = static_cast<float>(i);
            float r = radiusScale * std::sqrt(fi);
            float theta = fi * twistRad + time * 0.2f;

            float x = r * std::cos(theta);
            float z = r * std::sin(theta);
            float y = 0.0f;

            if (chanBuf && chanBuf->GetSampleCount() > 0) {
                size_t sampleIdx = static_cast<size_t>(i) % chanBuf->GetSampleCount();
                y += chanBuf->GetChannelData(0)[sampleIdx] * 2.0f;
            }

            glm::vec3 pos(x, y, z);
            float cosT = std::cos(theta);
            float sinT = std::sin(theta);
            glm::mat4 m(1.0f);
            m[0][0] = cosT * baseScale.x;
            m[0][2] = -sinT * baseScale.x;
            m[1][1] = baseScale.y;
            m[2][0] = sinT * baseScale.z;
            m[2][2] = cosT * baseScale.z;
            m[3] = glm::vec4(pos, 1.0f);

            InstanceData inst{};
            inst.transform = m;

            float prog = count > 1 ? static_cast<float>(i) / static_cast<float>(count - 1) : 0.0f;
            if (colorMode == 1) {
                inst.color = glm::mix(startColor, endColor, prog);
            } else if (colorMode == 2) {
                float dProg = maxR > 0.01f ? std::clamp(r / maxR, 0.0f, 1.0f) : 0.0f;
                inst.color = glm::mix(startColor, endColor, dProg);
            } else {
                inst.color = startColor;
            }

            instances.push_back(inst);
        }
    }

    outGeom.SetInstances(std::move(instances));
    SetOutputGeometry(std::move(outGeom));
    return true;
}

} // namespace nf
