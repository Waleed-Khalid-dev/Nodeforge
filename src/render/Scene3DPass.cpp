#include "Scene3DPass.h"
#include "../gpu/ShaderCompiler.h"
#include <spdlog/spdlog.h>
#include <cstring>

namespace nf {

static const char* s_vertShader3D = R"(
#version 450
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec4 inColor;
layout(location = 4) in vec4 inTangent;

layout(location = 0) out vec3 outWorldPos;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec2 outUV;
layout(location = 3) out vec4 outColor;

layout(push_constant) uniform PushConstants {
    mat4 mvp;
    mat4 model;
    vec4 baseColor;
    vec4 lightDir;
    vec4 lightColor;
    vec4 cameraPos;
    vec4 params;
} push;

void main() {
    vec4 worldPos = push.model * vec4(inPos, 1.0);
    outWorldPos = worldPos.xyz;
    outNormal = normalize(mat3(push.model) * inNormal);
    outUV = inUV;
    outColor = inColor * push.baseColor;
    gl_Position = push.mvp * vec4(inPos, 1.0);
}
)";

static const char* s_fragShader3D = R"(
#version 450
layout(location = 0) in vec3 inWorldPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec4 inColor;

layout(location = 0) out vec4 fragColor;

layout(push_constant) uniform PushConstants {
    mat4 mvp;
    mat4 model;
    vec4 baseColor;
    vec4 lightDir;
    vec4 lightColor;
    vec4 cameraPos;
    vec4 params; // x: shininess, y: unlit, z: shadingMode, w: pad
} push;

void main() {
    if (push.params.y > 0.5) { // Unlit
        fragColor = inColor;
        return;
    }

    vec3 N = normalize(inNormal);
    vec3 L = normalize(push.lightDir.xyz);
    vec3 V = normalize(push.cameraPos.xyz - inWorldPos);
    vec3 H = normalize(L + V);

    float diff = max(dot(N, L), 0.0);
    float spec = pow(max(dot(N, H), 0.0), max(push.params.x, 1.0)) * (diff > 0.0 ? 1.0 : 0.0);

    vec3 ambient = vec3(0.15) * inColor.rgb;
    vec3 diffuse = diff * push.lightColor.rgb * inColor.rgb;
    vec3 specular = spec * push.lightColor.rgb * 0.4;

    fragColor = vec4(ambient + diffuse + specular, inColor.a);
}
)";

Scene3DPass::Scene3DPass(gpu::Device* device, gpu::TexturePool* pool)
    : m_device(device), m_pool(pool) {
    InitPipeline();
}

Scene3DPass::~Scene3DPass() {
    CleanupPipeline();
}

void Scene3DPass::InitPipeline() {
    if (!m_device || m_device->GetDevice() == VK_NULL_HANDLE) return;

    VkDevice dev = m_device->GetDevice();

    // Push constant range
    VkPushConstantRange pcRange{
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
        .offset = 0,
        .size = sizeof(PushConstants3D)
    };

    VkPipelineLayoutCreateInfo layoutInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pushConstantRangeCount = 1,
        .pPushConstantRanges = &pcRange
    };

    vkCreatePipelineLayout(dev, &layoutInfo, nullptr, &m_pipelineLayout);
}

void Scene3DPass::CleanupPipeline() {
    if (!m_device || m_device->GetDevice() == VK_NULL_HANDLE) return;
    VkDevice dev = m_device->GetDevice();

    if (m_pipelineLayout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(dev, m_pipelineLayout, nullptr);
        m_pipelineLayout = VK_NULL_HANDLE;
    }
}

bool Scene3DPass::Execute(
    gpu::Texture2D* targetColor,
    const glm::mat4& viewMatrix,
    const glm::mat4& projMatrix,
    const glm::vec3& cameraPos,
    const std::vector<DrawCall3D>& drawCalls,
    const std::vector<LightData>& lights,
    const glm::vec4& /*clearColor*/,
    bool /*enableDepth*/,
    int /*cullMode*/
) {
    if (!targetColor) return false;

    // Evaluate primary light
    LightData primaryLight{};
    if (!lights.empty()) {
        primaryLight = lights[0];
    } else {
        primaryLight.direction = glm::vec3(0.577f, 0.577f, 0.577f);
        primaryLight.color = glm::vec3(1.0f);
        primaryLight.intensity = 1.0f;
    }

    // Execute draw call transforms
    for (const auto& dc : drawCalls) {
        if (!dc.geometry || dc.geometry->IsEmpty()) continue;

        PushConstants3D pc{};
        pc.model = dc.modelMatrix;
        pc.mvp = projMatrix * viewMatrix * dc.modelMatrix;
        pc.baseColor = dc.material ? dc.material->color : glm::vec4(1.0f);
        pc.lightDir = glm::vec4(primaryLight.direction, 0.0f);
        pc.lightColor = glm::vec4(primaryLight.color * primaryLight.intensity, 1.0f);
        pc.cameraPos = glm::vec4(cameraPos, 1.0f);
        pc.params.x = dc.material ? dc.material->shininess : 32.0f;
        pc.params.y = (dc.material && dc.material->type == MaterialType::Constant) ? 1.0f : 0.0f;
    }

    return true;
}

} // namespace nf
