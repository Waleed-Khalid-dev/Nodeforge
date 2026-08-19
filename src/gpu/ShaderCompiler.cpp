#include "ShaderCompiler.h"
#include <spdlog/spdlog.h>

namespace gpu {

std::vector<uint32_t> ShaderCompiler::CompileGLSL(const std::string& source, shaderc_shader_kind kind, const std::string& name, std::string* outError) {
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;
    options.SetOptimizationLevel(shaderc_optimization_level_performance);
    options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);

    shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(source, kind, name.c_str(), options);
    if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
        std::string err = result.GetErrorMessage();
        spdlog::error("GLSL compilation failed for '{}': {}", name, err);
        if (outError) *outError = err;
        return {};
    }

    return std::vector<uint32_t>(result.cbegin(), result.cend());
}

VkShaderModule ShaderCompiler::CreateShaderModule(VkDevice device, const std::vector<uint32_t>& spirv) {
    if (spirv.empty() || !device) return VK_NULL_HANDLE;

    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = spirv.size() * sizeof(uint32_t);
    createInfo.pCode = spirv.data();

    VkShaderModule module = VK_NULL_HANDLE;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &module) != VK_SUCCESS) {
        spdlog::error("Failed to create VkShaderModule");
        return VK_NULL_HANDLE;
    }
    return module;
}

VkShaderModule ShaderCompiler::CompileAndCreateModule(VkDevice device, const std::string& source, shaderc_shader_kind kind, const std::string& name, std::string* outError) {
    std::vector<uint32_t> spirv = CompileGLSL(source, kind, name, outError);
    if (spirv.empty()) return VK_NULL_HANDLE;
    return CreateShaderModule(device, spirv);
}

const char* ShaderCompiler::GetFullscreenTriangleVertexShader() {
    return R"(
#version 450
layout (location = 0) out vec2 outUV;
void main() {
    outUV = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
    gl_Position = vec4(outUV * 2.0f + -1.0f, 0.0f, 1.0f);
}
)";
}

const char* ShaderCompiler::GetConstantFragmentShader() {
    return R"(
#version 450
layout (location = 0) in vec2 inUV;
layout (location = 0) out vec4 outColor;
layout (push_constant) uniform PushConsts {
    vec4 color;
} pc;
void main() {
    outColor = pc.color;
}
)";
}

const char* ShaderCompiler::GetTransformFragmentShader() {
    return R"(
#version 450
layout (location = 0) in vec2 inUV;
layout (location = 0) out vec4 outColor;
layout (binding = 0) uniform sampler2D uSampler;
layout (push_constant) uniform PushConsts {
    vec2 translate;
    vec2 scale;
    vec2 pivot;
    float rotate;
} pc;
void main() {
    vec2 uv = inUV - pc.pivot;
    float cosA = cos(-pc.rotate);
    float sinA = sin(-pc.rotate);
    mat2 rot = mat2(cosA, -sinA, sinA, cosA);
    uv = rot * uv;
    uv = uv / max(vec2(0.0001), pc.scale);
    uv = uv + pc.pivot - pc.translate;
    outColor = texture(uSampler, uv);
}
)";
}

const char* ShaderCompiler::GetCompositeFragmentShader() {
    return R"(
#version 450
layout (location = 0) in vec2 inUV;
layout (location = 0) out vec4 outColor;
layout (binding = 0) uniform sampler2D uInput1;
layout (binding = 1) uniform sampler2D uInput2;
layout (push_constant) uniform PushConsts {
    int operation;
    float opacity;
} pc;
void main() {
    vec4 c1 = texture(uInput1, inUV);
    vec4 c2 = texture(uInput2, inUV) * pc.opacity;
    vec4 res = vec4(0.0);
    if (pc.operation == 0) { // Over (Alpha blend)
        res.rgb = c2.rgb + c1.rgb * (1.0 - c2.a);
        res.a = c2.a + c1.a * (1.0 - c2.a);
    } else if (pc.operation == 1) { // Add
        res = c1 + c2;
    } else if (pc.operation == 2) { // Multiply
        res = c1 * c2;
    } else if (pc.operation == 3) { // Subtract
        res = max(c1 - c2, vec4(0.0));
    } else if (pc.operation == 4) { // Screen
        res = 1.0 - (1.0 - c1) * (1.0 - c2);
    } else if (pc.operation == 5) { // Darken
        res = min(c1, c2);
    } else if (pc.operation == 6) { // Lighten
        res = max(c1, c2);
    } else {
        res = c1;
    }
    outColor = clamp(res, 0.0, 1.0);
}
)";
}

const char* ShaderCompiler::GetBlurFragmentShader() {
    return R"(
#version 450
layout (location = 0) in vec2 inUV;
layout (location = 0) out vec4 outColor;
layout (binding = 0) uniform sampler2D uSampler;
layout (push_constant) uniform PushConsts {
    vec2 direction;
    float radius;
} pc;
void main() {
    vec4 sum = vec4(0.0);
    float totalWeight = 0.0;
    int samples = int(clamp(pc.radius, 1.0, 32.0));
    for (int i = -samples; i <= samples; ++i) {
        float x = float(i);
        float weight = exp(-(x * x) / (2.0 * pc.radius * pc.radius + 0.0001));
        vec2 offset = float(i) * pc.direction;
        sum += texture(uSampler, inUV + offset) * weight;
        totalWeight += weight;
    }
    outColor = sum / max(0.0001, totalWeight);
}
)";
}

const char* ShaderCompiler::GetLevelFragmentShader() {
    return R"(
#version 450
layout (location = 0) in vec2 inUV;
layout (location = 0) out vec4 outColor;
layout (binding = 0) uniform sampler2D uSampler;
layout (push_constant) uniform PushConsts {
    float blackLevel;
    float whiteLevel;
    float brightness;
    float gamma;
    float contrast;
} pc;
void main() {
    vec4 c = texture(uSampler, inUV);
    vec3 rgb = c.rgb;
    float denom = max(0.0001, pc.whiteLevel - pc.blackLevel);
    rgb = clamp((rgb - vec3(pc.blackLevel)) / denom, 0.0, 1.0);
    rgb = pow(rgb, vec3(1.0 / max(0.01, pc.gamma)));
    rgb = (rgb - 0.5) * pc.contrast + 0.5;
    rgb = rgb * pc.brightness;
    outColor = vec4(clamp(rgb, 0.0, 1.0), c.a);
}
)";
}

const char* ShaderCompiler::GetResolutionFragmentShader() {
    return R"(
#version 450
layout (location = 0) in vec2 inUV;
layout (location = 0) out vec4 outColor;
layout (binding = 0) uniform sampler2D uSampler;
layout (push_constant) uniform PushConsts {
    vec2 uvScale;
    vec2 uvOffset;
} pc;
void main() {
    vec2 uv = inUV * pc.uvScale + pc.uvOffset;
    if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0) {
        outColor = vec4(0.0, 0.0, 0.0, 0.0);
    } else {
        outColor = texture(uSampler, uv);
    }
}
)";
}

const char* ShaderCompiler::GetNoiseComputeShader() {
    return R"(
#version 450
layout (local_size_x = 16, local_size_y = 16) in;
layout (binding = 0, rgba8) uniform writeonly image2D outImage;
layout (push_constant) uniform PushConsts {
    int width;
    int height;
    int noiseType;
    int octaves;
    float period;
    float roughness;
    float tx;
    float ty;
    float tz;
} pc;

float hash(vec2 p) {
    p = 50.0 * fract(p * 0.3183099 + vec2(0.71, 0.113));
    return -1.0 + 2.0 * fract(16.0 * p.x * p.y * (p.x + p.y));
}

float noise2D(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(mix(hash(i + vec2(0.0, 0.0)), hash(i + vec2(1.0, 0.0)), u.x),
               mix(hash(i + vec2(0.0, 1.0)), hash(i + vec2(1.0, 1.0)), u.x), u.y);
}

float fbm(vec2 p, int octaves, float roughness) {
    float v = 0.0;
    float a = 0.5;
    vec2 shift = vec2(100.0);
    mat2 rot = mat2(cos(0.5), sin(0.5), -sin(0.5), cos(0.5));
    for (int i = 0; i < octaves; ++i) {
        v += a * noise2D(p);
        p = rot * p * 2.0 + shift;
        a *= roughness;
    }
    return v;
}

void main() {
    ivec2 pixel = ivec2(gl_GlobalInvocationID.xy);
    if (pixel.x >= pc.width || pixel.y >= pc.height) return;

    vec2 uv = vec2(pixel) / vec2(max(1, pc.width), max(1, pc.height));
    vec2 p = (uv * pc.period) + vec2(pc.tx, pc.ty) + vec2(pc.tz * 0.1);

    float n = 0.0;
    if (pc.octaves > 1) {
        n = fbm(p, pc.octaves, pc.roughness);
    } else {
        n = noise2D(p);
    }

    float val = clamp((n * 0.5 + 0.5), 0.0, 1.0);
    imageStore(outImage, pixel, vec4(val, val, val, 1.0));
}
)";
}

} // namespace gpu
