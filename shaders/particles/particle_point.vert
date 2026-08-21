#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec4 inColor;

layout(push_constant) uniform PushConsts {
    mat4 viewProj;
    vec4 cameraPos;
    vec4 materialParams; // x: baseSize, y: sizeAttenuation (bool), z: softFalloff (bool), w: blendMode
    vec4 colorMultiplier;
} pc;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragUV;

void main() {
    gl_Position = pc.viewProj * vec4(inPosition, 1.0);

    float dist = length(pc.cameraPos.xyz - inPosition);
    float pSize = pc.materialParams.x * 50.0;
    if (pc.materialParams.y > 0.5) {
        pSize /= max(0.1, dist);
    }
    gl_PointSize = clamp(pSize, 1.0, 128.0);

    fragColor = inColor * pc.colorMultiplier;
    fragUV = inUV;
}
