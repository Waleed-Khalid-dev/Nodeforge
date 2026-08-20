#include "ConstantMatOp.h"
#include "../../gpu/Texture.h"

namespace nf {

ConstantMatOp::ConstantMatOp(NodeId id, const std::string& name)
    : MatOp(id, name, "ConstantMatOp") {
    m_colorMapPin = AddInputPin("color_map", PinType::Tex);
    m_outPin = AddOutputPin("output", PinType::Mat);

    SetParam("color", glm::vec4(1.0f));
    SetParam("wireframe", false);
    SetParam("use_vertex_color", true);

    m_materialData.type = MaterialType::Constant;
}

bool ConstantMatOp::Cook(const CookContext& /*context*/) {
    m_materialData.type = MaterialType::Constant;
    m_materialData.color = GetParam("color").Is<glm::vec4>() ? GetParam("color").Get<glm::vec4>() : glm::vec4(1.0f);
    m_materialData.wireframe = GetParam("wireframe").Is<bool>() ? GetParam("wireframe").Get<bool>() : false;
    m_materialData.useVertexColor = GetParam("use_vertex_color").Is<bool>() ? GetParam("use_vertex_color").Get<bool>() : true;

    if (m_colorMapPin && m_colorMapPin->GetValue().Is<std::shared_ptr<gpu::Texture2D>>()) {
        m_materialData.diffuseMap = m_colorMapPin->GetValue().Get<std::shared_ptr<gpu::Texture2D>>();
    } else {
        m_materialData.diffuseMap = nullptr;
    }

    return true;
}

} // namespace nf
