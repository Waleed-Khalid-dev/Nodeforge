#include "GLSLMatOp.h"
#include "../../gpu/Texture.h"

namespace nf {

GLSLMatOp::GLSLMatOp(NodeId id, const std::string& name)
    : MatOp(id, name, "GLSLMatOp") {
    m_map1Pin = AddInputPin("map1", PinType::Tex);
    m_map2Pin = AddInputPin("map2", PinType::Tex);
    m_outPin = AddOutputPin("output", PinType::Mat);

    SetParam("vertex_shader", std::string(
        "#version 450\n"
        "layout(location = 0) in vec3 inPos;\n"
        "layout(location = 1) in vec3 inNormal;\n"
        "layout(location = 2) in vec2 inUV;\n"
        "layout(location = 3) in vec4 inColor;\n"
        "layout(location = 0) out vec2 outUV;\n"
        "layout(location = 1) out vec3 outNormal;\n"
        "layout(push_constant) uniform PushConstants {\n"
        "    mat4 mvp;\n"
        "    mat4 model;\n"
        "} push;\n"
        "void main() {\n"
        "    gl_Position = push.mvp * vec4(inPos, 1.0);\n"
        "    outUV = inUV;\n"
        "    outNormal = mat3(push.model) * inNormal;\n"
        "}\n"
    ));

    SetParam("fragment_shader", std::string(
        "#version 450\n"
        "layout(location = 0) in vec2 inUV;\n"
        "layout(location = 1) in vec3 inNormal;\n"
        "layout(location = 0) out vec4 outColor;\n"
        "void main() {\n"
        "    vec3 norm = normalize(inNormal);\n"
        "    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));\n"
        "    float diff = max(dot(norm, lightDir), 0.0) * 0.7 + 0.3;\n"
        "    outColor = vec4(vec3(diff), 1.0);\n"
        "}\n"
    ));

    SetParam("wireframe", false);

    m_materialData.type = MaterialType::GLSL;
}

bool GLSLMatOp::Cook(const CookContext& /*context*/) {
    m_materialData.type = MaterialType::GLSL;
    m_materialData.customVertexShader = GetParam("vertex_shader").Is<std::string>() ? GetParam("vertex_shader").Get<std::string>() : "";
    m_materialData.customFragmentShader = GetParam("fragment_shader").Is<std::string>() ? GetParam("fragment_shader").Get<std::string>() : "";
    m_materialData.wireframe = GetParam("wireframe").Is<bool>() ? GetParam("wireframe").Get<bool>() : false;

    if (m_map1Pin && m_map1Pin->GetValue().Is<std::shared_ptr<gpu::Texture2D>>()) {
        m_materialData.diffuseMap = m_map1Pin->GetValue().Get<std::shared_ptr<gpu::Texture2D>>();
    } else {
        m_materialData.diffuseMap = nullptr;
    }

    if (m_map2Pin && m_map2Pin->GetValue().Is<std::shared_ptr<gpu::Texture2D>>()) {
        m_materialData.normalMap = m_map2Pin->GetValue().Get<std::shared_ptr<gpu::Texture2D>>();
    } else {
        m_materialData.normalMap = nullptr;
    }

    return true;
}

} // namespace nf
