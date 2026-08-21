#include "LaserPatternGeomOp.h"
#include "../../laser/LaserEngine.h"

namespace nf {

LaserPatternGeomOp::LaserPatternGeomOp(NodeId id, const std::string& name)
    : Node(id, name, "LaserPatternGeomOp") {
    m_outGeomPin = AddOutputPin("output", PinType::Geom);

    SetParam("pattern_type", static_cast<int32_t>(0)); // 0: Lissajous, 1: Spirograph, 2: Beam Fan, 3: Tunnel
    SetParam("frequency_a", 3.0f);
    SetParam("frequency_b", 2.0f);
    SetParam("phase", 0.0f);
    SetParam("beam_count", static_cast<int32_t>(8));
}

int32_t LaserPatternGeomOp::GetPatternType() const {
    return GetParam("pattern_type").Is<int32_t>() ? GetParam("pattern_type").Get<int32_t>() : 0;
}

float LaserPatternGeomOp::GetFrequencyA() const {
    return GetParam("frequency_a").Is<float>() ? GetParam("frequency_a").Get<float>() : 3.0f;
}

float LaserPatternGeomOp::GetFrequencyB() const {
    return GetParam("frequency_b").Is<float>() ? GetParam("frequency_b").Get<float>() : 2.0f;
}

float LaserPatternGeomOp::GetPhase() const {
    return GetParam("phase").Is<float>() ? GetParam("phase").Get<float>() : 0.0f;
}

int32_t LaserPatternGeomOp::GetBeamCount() const {
    return GetParam("beam_count").Is<int32_t>() ? GetParam("beam_count").Get<int32_t>() : 8;
}

bool LaserPatternGeomOp::Cook(const CookContext& /*context*/) {
    int type = GetPatternType();
    glm::vec4 cyan(0.0f, 0.8f, 1.0f, 1.0f);
    std::vector<LaserPoint> pts;

    if (type == 0) {
        pts = LaserEngine::Instance().GenerateLissajous(GetFrequencyA(), GetFrequencyB(), GetPhase(), 200, cyan);
    } else if (type == 1) {
        pts = LaserEngine::Instance().GenerateSpirograph(1.0f, 0.35f, 0.25f, 250, cyan);
    } else if (type == 2) {
        pts = LaserEngine::Instance().GenerateBeamFan(GetBeamCount(), 1.2f, cyan);
    } else {
        pts = LaserEngine::Instance().GenerateLissajous(4.0f, 4.0f, GetPhase(), 120, cyan);
    }

    m_geomData.Clear();
    for (const auto& p : pts) {
        Vertex v{};
        v.pos = glm::vec3(
            LaserPoint::DenormalizeCoord(p.x),
            LaserPoint::DenormalizeCoord(p.y),
            LaserPoint::DenormalizeCoord(p.z)
        );
        v.color = glm::vec4(p.r, p.g, p.b, p.intensity);
        m_geomData.GetVertices().push_back(v);
    }

    m_outGeomPin->SetValue(PinValue(m_geomData));
    return true;
}

} // namespace nf
