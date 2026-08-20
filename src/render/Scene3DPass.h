#pragma once

#include "../gpu/Device.h"
#include "../gpu/Texture.h"
#include "../gpu/TexturePool.h"
#include "../core/GeometryData.h"
#include "../operators/mat/MatOp.h"
#include "../operators/comp/LightComp.h"
#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace nf {

struct DrawCall3D {
    const GeometryData* geometry = nullptr;
    const MaterialData* material = nullptr;
    glm::mat4 modelMatrix{1.0f};
    std::vector<InstanceData> instances;
};

struct PushConstants3D {
    glm::mat4 mvp{1.0f};
    glm::mat4 model{1.0f};
    glm::vec4 baseColor{1.0f};
    glm::vec4 lightDir{0.577f, 0.577f, 0.577f, 0.0f};
    glm::vec4 lightColor{1.0f, 1.0f, 1.0f, 1.0f};
    glm::vec4 cameraPos{0.0f, 0.0f, 5.0f, 1.0f};
    glm::vec4 params{32.0f, 0.0f, 0.0f, 0.0f}; // x: shininess, y: unlit, z: shadingMode, w: pad
};

class Scene3DPass {
public:
    Scene3DPass(gpu::Device* device, gpu::TexturePool* pool);
    ~Scene3DPass();

    bool Execute(
        gpu::Texture2D* targetColor,
        const glm::mat4& viewMatrix,
        const glm::mat4& projMatrix,
        const glm::vec3& cameraPos,
        const std::vector<DrawCall3D>& drawCalls,
        const std::vector<LightData>& lights,
        const glm::vec4& clearColor = glm::vec4(0.05f, 0.05f, 0.08f, 1.0f),
        bool enableDepth = true,
        int cullMode = 1 // 0: None, 1: Back, 2: Front
    );

private:
    void InitPipeline();
    void CleanupPipeline();

    gpu::Device* m_device = nullptr;
    gpu::TexturePool* m_pool = nullptr;

    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkPipeline m_litPipeline = VK_NULL_HANDLE;
    VkPipeline m_unlitPipeline = VK_NULL_HANDLE;
    VkPipeline m_wireframePipeline = VK_NULL_HANDLE;
};

} // namespace nf
