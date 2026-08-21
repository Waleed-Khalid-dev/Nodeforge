#pragma once

#include "LaserPoint.h"
#include <vector>
#include <string>

namespace nf {

struct LaserTelemetry {
    float bufferFill = 0.5f;
    int currentPPS = 30000;
    int droppedPoints = 0;
    bool shutterOpen = true;
};

class LaserEngine {
public:
    static LaserEngine& Instance();

    std::vector<LaserPoint> OptimizePath(
        const std::vector<glm::vec3>& vertices,
        int blankDelay = 8,
        int cornerDwell = 4,
        const glm::vec4& color = glm::vec4(0.0f, 1.0f, 0.2f, 1.0f)
    );

    std::vector<LaserPoint> GenerateLissajous(
        float freqA,
        float freqB,
        float phase,
        int pointCount,
        const glm::vec4& color
    );

    std::vector<LaserPoint> GenerateSpirograph(
        float R,
        float r,
        float p,
        int pointCount,
        const glm::vec4& color
    );

    std::vector<LaserPoint> GenerateBeamFan(
        int beamCount,
        float angleSpread,
        const glm::vec4& color
    );

    LaserTelemetry TransmitPoints(
        const std::vector<LaserPoint>& points,
        int targetPPS,
        bool safetyShutter,
        int dacType
    );

private:
    LaserEngine() = default;
    ~LaserEngine() = default;

    int m_bufferedPointCount = 0;
};

} // namespace nf
