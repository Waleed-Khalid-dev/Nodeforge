#pragma once

#include "../gpu/Device.h"
#include "../gpu/Texture.h"
#include "../gpu/TexturePool.h"
#include "../media/WarpMesh.h"
#include <glm/glm.hpp>
#include <memory>

namespace nf {

struct WarpBlendPushConstants {
    glm::vec4 blendEdges{0.0f};  // x: left, y: right, z: top, w: bottom
    glm::vec4 blendParams{2.2f, 0.0f, 0.0f, 0.0f}; // x: gamma, y: black_level, z: test_pattern, w: pad
};

class WarpBlendPass {
public:
    WarpBlendPass(gpu::Device* device, gpu::TexturePool* pool);
    ~WarpBlendPass();

    bool Execute(
        gpu::Texture2D* target,
        gpu::Texture2D* source,
        const WarpMesh& warpMesh,
        const glm::vec4& blendEdges,
        float gamma = 2.2f,
        float blackLevel = 0.0f,
        int testPattern = 0
    );

private:
    void InitPipeline();
    void CleanupPipeline();

    gpu::Device* m_device = nullptr;
    gpu::TexturePool* m_pool = nullptr;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
};

} // namespace nf
