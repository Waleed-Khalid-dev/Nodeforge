#include "WarpBlendPass.h"
#include <spdlog/spdlog.h>

namespace nf {

WarpBlendPass::WarpBlendPass(gpu::Device* device, gpu::TexturePool* pool)
    : m_device(device), m_pool(pool) {
    InitPipeline();
}

WarpBlendPass::~WarpBlendPass() {
    CleanupPipeline();
}

void WarpBlendPass::InitPipeline() {
    if (!m_device || m_device->GetDevice() == VK_NULL_HANDLE) return;

    VkDevice dev = m_device->GetDevice();

    VkPushConstantRange pcRange{
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
        .offset = 0,
        .size = sizeof(WarpBlendPushConstants)
    };

    VkPipelineLayoutCreateInfo layoutInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pushConstantRangeCount = 1,
        .pPushConstantRanges = &pcRange
    };

    vkCreatePipelineLayout(dev, &layoutInfo, nullptr, &m_pipelineLayout);
}

void WarpBlendPass::CleanupPipeline() {
    if (!m_device || m_device->GetDevice() == VK_NULL_HANDLE) return;
    VkDevice dev = m_device->GetDevice();

    if (m_pipelineLayout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(dev, m_pipelineLayout, nullptr);
        m_pipelineLayout = VK_NULL_HANDLE;
    }
}

bool WarpBlendPass::Execute(
    gpu::Texture2D* target,
    gpu::Texture2D* /*source*/,
    const WarpMesh& warpMesh,
    const glm::vec4& blendEdges,
    float gamma,
    float blackLevel,
    int testPattern
) {
    if (!target) return false;

    // Generate mesh vertices for warped quad rasterization
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    warpMesh.GenerateGeometry(vertices, indices, 32, 32);

    WarpBlendPushConstants pc{};
    pc.blendEdges = blendEdges;
    pc.blendParams = glm::vec4(gamma, blackLevel, static_cast<float>(testPattern), 0.0f);

    return true;
}

} // namespace nf
