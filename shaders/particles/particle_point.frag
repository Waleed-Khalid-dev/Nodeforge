#version 450

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragUV;

layout(push_constant) uniform PushConsts {
    mat4 viewProj;
    vec4 cameraPos;
    vec4 materialParams; // x: baseSize, y: sizeAttenuation (bool), z: softFalloff (bool), w: blendMode
    vec4 colorMultiplier;
} pc;

layout(location = 0) out vec4 outColor;

void main() {
    vec2 coord = gl_PointCoord - vec2(0.5);
    float distSq = dot(coord, coord);

    if (distSq > 0.25) {
        discard;
    }

    float alpha = 1.0;
    if (pc.materialParams.z > 0.5) {
        alpha = exp(-distSq * 16.0); // Soft Gaussian circular particle falloff
    }

    outColor = vec4(fragColor.rgb, fragColor.a * alpha);
}
