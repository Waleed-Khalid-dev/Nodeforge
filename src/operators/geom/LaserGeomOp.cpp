#include "LaserGeomOp.h"
#include "../../laser/LaserEngine.h"

namespace nf {

LaserGeomOp::LaserGeomOp(NodeId id, const std::string& name)
    : Node(id, name, "LaserGeomOp") {
    m_inGeomPin = AddInputPin("geometry", PinType::Geom);
    m_outGeomPin = AddOutputPin("output", PinType::Geom);
    m_outPointsPin = AddOutputPin("out_points", PinType::Chan);

    SetParam("target_pps", static_cast<int32_t>(30000));
    SetParam("blank_delay", static_cast<int32_t>(8));
    SetParam("corner_dwell", static_cast<int32_t>(4));
    SetParam("color_override", glm::vec4(0.0f, 1.0f, 0.2f, 1.0f));

    m_pointsBuffer.Resize(7, 1);
    m_pointsBuffer.SetChannelNames({"x", "y", "z", "r", "g", "b", "blank"});
}

int32_t LaserGeomOp::GetTargetPPS() const {
    return GetParam("target_pps").Is<int32_t>() ? GetParam("target_pps").Get<int32_t>() : 30000;
}

int32_t LaserGeomOp::GetBlankDelay() const {
    return GetParam("blank_delay").Is<int32_t>() ? GetParam("blank_delay").Get<int32_t>() : 8;
}

int32_t LaserGeomOp::GetCornerDwell() const {
    return GetParam("corner_dwell").Is<int32_t>() ? GetParam("corner_dwell").Get<int32_t>() : 4;
}

glm::vec4 LaserGeomOp::GetColorOverride() const {
    return GetParam("color_override").Is<glm::vec4>() ? GetParam("color_override").Get<glm::vec4>() : glm::vec4(0.0f, 1.0f, 0.2f, 1.0f);
}

bool LaserGeomOp::Cook(const CookContext& /*context*/) {
    std::vector<glm::vec3> positions;

    if (m_inGeomPin && m_inGeomPin->GetValue().Is<GeometryData>()) {
        const auto& inGeom = m_inGeomPin->GetValue().Get<GeometryData>();
        const auto& vertices = inGeom.GetVertices();
        positions.reserve(vertices.size());
        for (const auto& v : vertices) {
            positions.push_back(v.pos);
        }
    } else {
        // Default circle contour if no input geometry
        positions.reserve(32);
        for (int i = 0; i < 32; ++i) {
            float angle = static_cast<float>(i) * (6.2831853f / 32.0f);
            positions.emplace_back(std::cos(angle) * 0.8f, std::sin(angle) * 0.8f, 0.0f);
        }
    }

    auto laserPoints = LaserEngine::Instance().OptimizePath(
        positions,
        GetBlankDelay(),
        GetCornerDwell(),
        GetColorOverride()
    );

    // Build geometry mesh
    m_outGeom.Clear();
    for (const auto& lp : laserPoints) {
        Vertex vert{};
        vert.pos = glm::vec3(
            LaserPoint::DenormalizeCoord(lp.x),
            LaserPoint::DenormalizeCoord(lp.y),
            LaserPoint::DenormalizeCoord(lp.z)
        );
        vert.color = lp.isBlanked ? glm::vec4(0.0f) : glm::vec4(lp.r, lp.g, lp.b, lp.intensity);
        m_outGeom.GetVertices().push_back(vert);
    }

    // Build channel buffer
    size_t ptCount = std::max(laserPoints.size(), size_t(1));
    m_pointsBuffer.Resize(7, static_cast<uint32_t>(ptCount));
    m_pointsBuffer.SetChannelNames({"x", "y", "z", "r", "g", "b", "blank"});

    for (size_t i = 0; i < laserPoints.size(); ++i) {
        const auto& lp = laserPoints[i];
        m_pointsBuffer.GetChannelData(0)[i] = LaserPoint::DenormalizeCoord(lp.x);
        m_pointsBuffer.GetChannelData(1)[i] = LaserPoint::DenormalizeCoord(lp.y);
        m_pointsBuffer.GetChannelData(2)[i] = LaserPoint::DenormalizeCoord(lp.z);
        m_pointsBuffer.GetChannelData(3)[i] = lp.r;
        m_pointsBuffer.GetChannelData(4)[i] = lp.g;
        m_pointsBuffer.GetChannelData(5)[i] = lp.b;
        m_pointsBuffer.GetChannelData(6)[i] = lp.isBlanked ? 1.0f : 0.0f;
    }

    m_outGeomPin->SetValue(PinValue(m_outGeom));
    m_outPointsPin->SetValue(PinValue(m_pointsBuffer));
    return true;
}

} // namespace nf
