#pragma once

#include "PoseKeypoints.h"
#include <vector>
#include <memory>
#include <string>

namespace nf {

class ONNXInferenceEngine {
public:
    static ONNXInferenceEngine& Instance();

    bool Initialize(bool emulateHeadless = true);
    void Shutdown();
    bool IsActive() const { return m_initialized; }

    // Tensor conversion
    static std::vector<float> ConvertRGBAtoNCHW(const uint8_t* rgba, int width, int height);
    static void ConvertNCHWtoRGBA(const float* nchw, int width, int height, uint8_t* outRgba);

    // Neural transforms
    glm::vec4 ApplyStylePresetColor(const glm::vec4& inColor, int stylePreset, float weight);
    Pose17 EstimatePose(float timeSeconds);
    float ComputeSegmentationAlpha(const glm::vec2& uv, float timeSeconds, float threshold);

private:
    ONNXInferenceEngine() = default;
    ~ONNXInferenceEngine() = default;

    bool m_initialized = false;
    bool m_isEmulated = true;
};

} // namespace nf
