#pragma once

#include "../../graph/Node.h"
#include <memory>
#include <string>
#include <glm/glm.hpp>

namespace gpu {
    class Texture2D;
}

namespace nf {

enum class MaterialType {
    Constant,
    Phong,
    PBR,
    GLSL
};

struct MaterialData {
    MaterialType type = MaterialType::Constant;
    glm::vec4 color{1.0f};
    glm::vec4 specularColor{1.0f};
    glm::vec4 ambientColor{0.1f, 0.1f, 0.1f, 1.0f};
    glm::vec4 emissiveColor{0.0f, 0.0f, 0.0f, 1.0f};
    float shininess = 32.0f;
    bool wireframe = false;
    bool useVertexColor = true;

    std::shared_ptr<gpu::Texture2D> diffuseMap;
    std::shared_ptr<gpu::Texture2D> normalMap;

    std::string customVertexShader;
    std::string customFragmentShader;
};

class MatOp : public Node {
public:
    MatOp(NodeId id, const std::string& name, const std::string& typeName);
    virtual ~MatOp() = default;

    const MaterialData& GetMaterialData() const { return m_materialData; }
    void SetMaterialData(const MaterialData& mat) { m_materialData = mat; }

protected:
    MaterialData m_materialData;
    Pin* m_outPin = nullptr;
};

} // namespace nf
