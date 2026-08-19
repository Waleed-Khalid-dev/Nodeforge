#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <shaderc/shaderc.hpp>

namespace gpu {

class ShaderCompiler {
public:
    static std::vector<uint32_t> CompileGLSL(const std::string& source, shaderc_shader_kind kind, const std::string& name = "shader", std::string* outError = nullptr);
    static VkShaderModule CreateShaderModule(VkDevice device, const std::vector<uint32_t>& spirv);
    static VkShaderModule CompileAndCreateModule(VkDevice device, const std::string& source, shaderc_shader_kind kind, const std::string& name = "shader", std::string* outError = nullptr);

    // Standard embedded shader sources
    static const char* GetFullscreenTriangleVertexShader();
    static const char* GetConstantFragmentShader();
    static const char* GetTransformFragmentShader();
    static const char* GetCompositeFragmentShader();
    static const char* GetBlurFragmentShader();
    static const char* GetLevelFragmentShader();
    static const char* GetResolutionFragmentShader();
    static const char* GetNoiseComputeShader();
};

} // namespace gpu
