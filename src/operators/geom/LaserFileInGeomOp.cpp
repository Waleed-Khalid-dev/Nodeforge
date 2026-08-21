#include "LaserFileInGeomOp.h"

namespace nf {

LaserFileInGeomOp::LaserFileInGeomOp(NodeId id, const std::string& name)
    : Node(id, name, "LaserFileInGeomOp") {
    m_outGeomPin = AddOutputPin("output", PinType::Geom);

    SetParam("file_path", std::string(""));
    SetParam("play_mode", static_cast<int32_t>(0)); // 0: Loop, 1: Hold, 2: Scrub
    SetParam("speed", 1.0f);
}

std::string LaserFileInGeomOp::GetFilePath() const {
    return GetParam("file_path").Is<std::string>() ? GetParam("file_path").Get<std::string>() : "";
}

int32_t LaserFileInGeomOp::GetPlayMode() const {
    return GetParam("play_mode").Is<int32_t>() ? GetParam("play_mode").Get<int32_t>() : 0;
}

float LaserFileInGeomOp::GetSpeed() const {
    return GetParam("speed").Is<float>() ? GetParam("speed").Get<float>() : 1.0f;
}

bool LaserFileInGeomOp::Cook(const CookContext& /*context*/) {
    // In headless mock / file load, generate baseline circle frame if file path is empty
    m_geomData.Clear();
    for (int i = 0; i < 48; ++i) {
        float angle = static_cast<float>(i) * (6.2831853f / 48.0f);
        Vertex v{};
        v.pos = glm::vec3(std::cos(angle) * 0.9f, std::sin(angle) * 0.9f, 0.0f);
        v.color = glm::vec4(1.0f, 0.2f, 0.8f, 1.0f);
        m_geomData.GetVertices().push_back(v);
    }

    m_outGeomPin->SetValue(PinValue(m_geomData));
    return true;
}

} // namespace nf
