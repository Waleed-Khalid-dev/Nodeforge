#include "ONNXInferenceEngine.h"
#include <cmath>
#include <algorithm>

namespace nf {

ONNXInferenceEngine& ONNXInferenceEngine::Instance() {
    static ONNXInferenceEngine instance;
    return instance;
}

bool ONNXInferenceEngine::Initialize(bool emulateHeadless) {
    m_isEmulated = emulateHeadless;
    m_initialized = true;
    return true;
}

void ONNXInferenceEngine::Shutdown() {
    m_initialized = false;
}

std::vector<float> ONNXInferenceEngine::ConvertRGBAtoNCHW(const uint8_t* rgba, int width, int height) {
    size_t pixelCount = static_cast<size_t>(width * height);
    std::vector<float> nchw(pixelCount * 3);

    const float mean[3] = {0.485f, 0.456f, 0.406f};
    const float stdDev[3] = {0.229f, 0.224f, 0.225f};

    for (size_t i = 0; i < pixelCount; ++i) {
        float r = static_cast<float>(rgba[i * 4 + 0]) / 255.0f;
        float g = static_cast<float>(rgba[i * 4 + 1]) / 255.0f;
        float b = static_cast<float>(rgba[i * 4 + 2]) / 255.0f;

        nchw[0 * pixelCount + i] = (r - mean[0]) / stdDev[0];
        nchw[1 * pixelCount + i] = (g - mean[1]) / stdDev[1];
        nchw[2 * pixelCount + i] = (b - mean[2]) / stdDev[2];
    }
    return nchw;
}

void ONNXInferenceEngine::ConvertNCHWtoRGBA(const float* nchw, int width, int height, uint8_t* outRgba) {
    size_t pixelCount = static_cast<size_t>(width * height);
    const float mean[3] = {0.485f, 0.456f, 0.406f};
    const float stdDev[3] = {0.229f, 0.224f, 0.225f};

    for (size_t i = 0; i < pixelCount; ++i) {
        float r = nchw[0 * pixelCount + i] * stdDev[0] + mean[0];
        float g = nchw[1 * pixelCount + i] * stdDev[1] + mean[1];
        float b = nchw[2 * pixelCount + i] * stdDev[2] + mean[2];

        outRgba[i * 4 + 0] = static_cast<uint8_t>(std::clamp(r * 255.0f, 0.0f, 255.0f));
        outRgba[i * 4 + 1] = static_cast<uint8_t>(std::clamp(g * 255.0f, 0.0f, 255.0f));
        outRgba[i * 4 + 2] = static_cast<uint8_t>(std::clamp(b * 255.0f, 0.0f, 255.0f));
        outRgba[i * 4 + 3] = 255;
    }
}

glm::vec4 ONNXInferenceEngine::ApplyStylePresetColor(const glm::vec4& inColor, int stylePreset, float weight) {
    glm::vec4 styled = inColor;

    switch (stylePreset) {
        case 0: // Candy
            styled = glm::vec4(inColor.r * 1.3f, inColor.g * 0.7f, inColor.b * 1.4f, inColor.a);
            break;
        case 1: // Mosaic
            styled = glm::vec4(inColor.b, inColor.r, inColor.g, inColor.a);
            break;
        case 2: // Rain Princess
            styled = glm::vec4(inColor.r * 0.5f, inColor.g * 0.9f, inColor.b * 1.5f, inColor.a);
            break;
        case 3: // Udnie
            styled = glm::vec4(inColor.g * 1.2f, inColor.r * 0.8f, inColor.b * 0.9f, inColor.a);
            break;
        case 4: // Cyberpunk Neon
            styled = glm::vec4(inColor.r * 1.5f + 0.1f, inColor.g * 0.2f, inColor.b * 1.8f + 0.2f, inColor.a);
            break;
        case 5: // Ink Wash
            {
                float lum = inColor.r * 0.299f + inColor.g * 0.587f + inColor.b * 0.114f;
                styled = glm::vec4(lum, lum, lum, inColor.a);
            }
            break;
        default:
            break;
    }

    return glm::mix(inColor, styled, std::clamp(weight, 0.0f, 1.0f));
}

Pose17 ONNXInferenceEngine::EstimatePose(float timeSeconds) {
    Pose17 pose{};
    float sway = std::sin(timeSeconds * 2.0f) * 0.1f;
    float handWave = std::cos(timeSeconds * 3.0f) * 0.2f;

    // Head
    pose.keypoints[0].position = glm::vec2(0.5f + sway * 0.5f, 0.2f); // Nose
    pose.keypoints[1].position = glm::vec2(0.48f + sway * 0.5f, 0.18f); // Left Eye
    pose.keypoints[2].position = glm::vec2(0.52f + sway * 0.5f, 0.18f); // Right Eye
    pose.keypoints[3].position = glm::vec2(0.45f + sway * 0.5f, 0.2f); // Left Ear
    pose.keypoints[4].position = glm::vec2(0.55f + sway * 0.5f, 0.2f); // Right Ear

    // Torso / Shoulders
    pose.keypoints[5].position = glm::vec2(0.4f + sway, 0.35f); // Left Shoulder
    pose.keypoints[6].position = glm::vec2(0.6f + sway, 0.35f); // Right Shoulder

    // Arms
    pose.keypoints[7].position = glm::vec2(0.35f + sway, 0.5f); // Left Elbow
    pose.keypoints[8].position = glm::vec2(0.65f + sway, 0.5f); // Right Elbow
    pose.keypoints[9].position = glm::vec2(0.3f + sway, 0.65f + handWave); // Left Wrist
    pose.keypoints[10].position = glm::vec2(0.7f + sway, 0.65f - handWave); // Right Wrist

    // Hips & Legs
    pose.keypoints[11].position = glm::vec2(0.45f + sway, 0.6f); // Left Hip
    pose.keypoints[12].position = glm::vec2(0.55f + sway, 0.6f); // Right Hip
    pose.keypoints[13].position = glm::vec2(0.44f + sway, 0.78f); // Left Knee
    pose.keypoints[14].position = glm::vec2(0.56f + sway, 0.78f); // Right Knee
    pose.keypoints[15].position = glm::vec2(0.43f + sway, 0.95f); // Left Ankle
    pose.keypoints[16].position = glm::vec2(0.57f + sway, 0.95f); // Right Ankle

    for (size_t i = 0; i < 17; ++i) {
        pose.keypoints[i].confidence = 0.95f;
    }
    return pose;
}

float ONNXInferenceEngine::ComputeSegmentationAlpha(const glm::vec2& uv, float timeSeconds, float threshold) {
    float centerDist = glm::length(uv - glm::vec2(0.5f + std::sin(timeSeconds) * 0.05f, 0.5f));
    float prob = std::clamp(1.0f - centerDist * 2.2f, 0.0f, 1.0f);
    return (prob >= threshold) ? 1.0f : 0.0f;
}

} // namespace nf
