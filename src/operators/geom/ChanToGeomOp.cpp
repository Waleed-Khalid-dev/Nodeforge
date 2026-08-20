#include "ChanToGeomOp.h"
#include "../../core/ChannelBuffer.h"

namespace nf {

ChanToGeomOp::ChanToGeomOp(NodeId id, const std::string& name)
    : GeomOp(id, name, "ChanToGeomOp") {
    m_inPin = AddInputPin("input", PinType::Chan);
    m_outPin = AddOutputPin("output", PinType::Geom);

    SetParam("chan_x", std::string("tx"));
    SetParam("chan_y", std::string("ty"));
    SetParam("chan_z", std::string("tz"));
    SetParam("primitive_type", static_cast<int32_t>(0)); // 0: Points, 1: Line Strip, 2: Triangles
}

bool ChanToGeomOp::Cook(const CookContext& /*context*/) {
    if (!m_inPin || !m_inPin->GetValue().Is<ChannelBuffer>()) {
        SetOutputGeometry(GeometryData{});
        return true;
    }

    const auto& buf = m_inPin->GetValue().Get<ChannelBuffer>();
    if (buf.IsEmpty()) {
        SetOutputGeometry(GeometryData{});
        return true;
    }

    std::string cx = GetParam("chan_x").Is<std::string>() ? GetParam("chan_x").Get<std::string>() : "tx";
    std::string cy = GetParam("chan_y").Is<std::string>() ? GetParam("chan_y").Get<std::string>() : "ty";
    std::string cz = GetParam("chan_z").Is<std::string>() ? GetParam("chan_z").Get<std::string>() : "tz";
    int32_t primType = GetParam("primitive_type").Is<int32_t>() ? GetParam("primitive_type").Get<int32_t>() : 0;

    size_t samples = buf.GetSampleCount();
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    vertices.reserve(samples);

    for (size_t s = 0; s < samples; ++s) {
        float px = buf.GetSample(cx, s);
        float py = buf.GetSample(cy, s);
        float pz = buf.GetSample(cz, s);

        float r = buf.GetSample("r", s);
        float g = buf.GetSample("g", s);
        float b = buf.GetSample("b", s);
        float a = buf.GetSample("a", s);
        if (a == 0.0f && r == 0.0f && g == 0.0f && b == 0.0f) {
            r = g = b = a = 1.0f;
        }

        vertices.push_back(Vertex{
            .pos = glm::vec3(px, py, pz),
            .normal = glm::vec3(0, 1, 0),
            .uv = glm::vec2(static_cast<float>(s) / samples, 0.0f),
            .color = glm::vec4(r, g, b, a),
            .tangent = glm::vec4(1, 0, 0, 1)
        });
    }

    if (primType == 1) { // Line strip -> as triangles/lines
        for (size_t i = 0; i + 1 < samples; ++i) {
            indices.push_back(static_cast<uint32_t>(i));
            indices.push_back(static_cast<uint32_t>(i + 1));
            indices.push_back(static_cast<uint32_t>(i + 1));
        }
    } else if (primType == 2) { // Triangles
        for (size_t i = 0; i + 2 < samples; i += 3) {
            indices.push_back(static_cast<uint32_t>(i));
            indices.push_back(static_cast<uint32_t>(i + 1));
            indices.push_back(static_cast<uint32_t>(i + 2));
        }
    } else { // Points -> quad sprite billboard triangles
        for (size_t i = 0; i < samples; ++i) {
            indices.push_back(static_cast<uint32_t>(i));
        }
    }

    GeometryData outGeom(std::move(vertices), std::move(indices));
    SetOutputGeometry(outGeom);
    return true;
}

} // namespace nf
