#include "PhongMatOp.h"
#include "../../gpu/Texture.h"

namespace nf {

PhongMatOp::PhongMatOp(NodeId id, const std::string& name)
    : MatOp(id, name, "PhongMatOp") {
    m_diffuseMapPin = AddInputPin("diffuse_map", PinType::Tex);
    m_normalMapPin = AddInputPin("normal_map", PinType::Tex);
    m_outPin = AddOutputPin("output", PinType::Mat);

    SetParam("diffuse_color", glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));
    SetParam("specular_color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    SetParam("ambient_color", glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
    SetParam("emissive_color", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    SetParam("shininess", 32.0f);
    SetParam("wireframe", false);

    m_materialData.type = MaterialType::Phong;
}

bool PhongMatOp::Cook(const CookContext& /*context*/) {
    m_materialData.type = MaterialType::Phong;
    m_materialData.color = GetParam("diffuse_color").Is<glm::vec4>() ? GetParam("diffuse_color").Get<glm::vec4>() : glm::vec4(0.8f);
    m_materialData.specularColor = GetParam("specular_color").Is<glm::vec4>() ? GetParam("specular_color").Get<glm::vec4>() : glm::vec4(1.0f);
    m_materialData.ambientColor = GetParam("ambient_color").Is<glm::vec4>() ? GetParam("ambient_color").Get<glm::vec4>() : glm::vec4(0.1f);
    m_materialData.emissiveColor = GetParam("emissive_color").Is<glm::vec4>() ? GetParam("emissive_color").Get<glm::vec4>() : glm::vec4(0.0f);
    m_materialData.shininess = GetParam("shininess").Is<float>() ? GetParam("shininess").Get<float>() : 32.0f;
    m_materialData.wireframe = GetParam("wireframe").Is<bool>() ? GetParam("wireframe").Get<bool>() : false;

    if (m_diffuseMapPin && m_diffuseMapPin->GetValue().Is<std::shared_ptr<gpu::Texture2D>>()) {
        m_materialData.diffuseMap = m_diffuseMapPin->GetValue().Get<std::shared_ptr<gpu::Texture2D>>();
    } else {
        m_materialData.diffuseMap = nullptr;
    }

    if (m_normalMapPin && m_normalMapPin->GetValue().Is<std::shared_ptr<gpu::Texture2D>>()) {
        m_materialData.normalMap = m_normalMapPin->GetValue().Get<std::shared_ptr<gpu::Texture2D>>();
    } else {
        m_materialData.normalMap = nullptr;
    }

    return true;
}

} // namespace nf
