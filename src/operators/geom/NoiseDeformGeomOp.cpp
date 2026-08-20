#include "NoiseDeformGeomOp.h"

namespace nf {

NoiseDeformGeomOp::NoiseDeformGeomOp(NodeId id, const std::string& name)
    : GeomOp(id, name, "NoiseDeformGeomOp") {
    m_inPin = AddInputPin("input", PinType::Geom);
    m_outPin = AddOutputPin("output", PinType::Geom);

    SetParam("amplitude", 0.2f);
    SetParam("frequency", 1.0f);
    SetParam("offset", glm::vec3(0.0f));
    SetParam("along_normal", true);
}

bool NoiseDeformGeomOp::Cook(const CookContext& /*context*/) {
    const GeometryData* inGeom = GetInputGeometry(0);
    if (!inGeom || inGeom->IsEmpty()) {
        SetOutputGeometry(GeometryData{});
        return true;
    }

    float amp = GetParam("amplitude").Is<float>() ? GetParam("amplitude").Get<float>() : 0.2f;
    float freq = GetParam("frequency").Is<float>() ? GetParam("frequency").Get<float>() : 1.0f;
    glm::vec3 offset = GetParam("offset").Is<glm::vec3>() ? GetParam("offset").Get<glm::vec3>() : glm::vec3(0.0f);
    bool alongNorm = GetParam("along_normal").Is<bool>() ? GetParam("along_normal").Get<bool>() : true;

    GeometryData outGeom = *inGeom;
    outGeom.DeformNoise(amp, freq, offset, alongNorm);
    SetOutputGeometry(outGeom);
    return true;
}

} // namespace nf
