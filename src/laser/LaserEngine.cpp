#include "LaserEngine.h"
#include <cmath>

namespace nf {

LaserEngine& LaserEngine::Instance() {
    static LaserEngine instance;
    return instance;
}

std::vector<LaserPoint> LaserEngine::OptimizePath(
    const std::vector<glm::vec3>& vertices,
    int blankDelay,
    int cornerDwell,
    const glm::vec4& color
) {
    std::vector<LaserPoint> result;
    if (vertices.empty()) return result;

    result.reserve(vertices.size() * 2 + blankDelay * 2);

    for (size_t i = 0; i < vertices.size(); ++i) {
        const auto& v = vertices[i];

        // If starting a new line / disconnected contour, insert blanking points
        if (i == 0 && blankDelay > 0) {
            for (int b = 0; b < blankDelay; ++b) {
                LaserPoint pt;
                pt.x = LaserPoint::NormalizeCoord(v.x);
                pt.y = LaserPoint::NormalizeCoord(v.y);
                pt.z = LaserPoint::NormalizeCoord(v.z);
                pt.r = 0.0f;
                pt.g = 0.0f;
                pt.b = 0.0f;
                pt.isBlanked = true;
                result.push_back(pt);
            }
        }

        LaserPoint pt;
        pt.x = LaserPoint::NormalizeCoord(v.x);
        pt.y = LaserPoint::NormalizeCoord(v.y);
        pt.z = LaserPoint::NormalizeCoord(v.z);
        pt.r = color.r;
        pt.g = color.g;
        pt.b = color.b;
        pt.intensity = color.a;
        pt.isBlanked = false;
        result.push_back(pt);

        // Corner dwell at vertices
        if (cornerDwell > 0) {
            for (int d = 0; d < cornerDwell; ++d) {
                result.push_back(pt);
            }
        }
    }

    return result;
}

std::vector<LaserPoint> LaserEngine::GenerateLissajous(
    float freqA,
    float freqB,
    float phase,
    int pointCount,
    const glm::vec4& color
) {
    std::vector<LaserPoint> points;
    points.reserve(pointCount);

    const float twoPi = 6.28318530718f;
    for (int i = 0; i < pointCount; ++i) {
        float t = (static_cast<float>(i) / static_cast<float>(pointCount)) * twoPi;
        float x = std::sin(freqA * t + phase);
        float y = std::sin(freqB * t);

        LaserPoint pt;
        pt.x = LaserPoint::NormalizeCoord(x);
        pt.y = LaserPoint::NormalizeCoord(y);
        pt.z = 0;
        pt.r = color.r;
        pt.g = color.g;
        pt.b = color.b;
        pt.intensity = color.a;
        pt.isBlanked = false;
        points.push_back(pt);
    }
    return points;
}

std::vector<LaserPoint> LaserEngine::GenerateSpirograph(
    float R,
    float r,
    float p,
    int pointCount,
    const glm::vec4& color
) {
    std::vector<LaserPoint> points;
    points.reserve(pointCount);

    const float twoPi = 6.28318530718f;
    float maxRadius = R - r + p;
    if (maxRadius < 0.001f) maxRadius = 1.0f;

    for (int i = 0; i < pointCount; ++i) {
        float t = (static_cast<float>(i) / static_cast<float>(pointCount)) * twoPi * 4.0f;
        float x = (R - r) * std::cos(t) + p * std::cos((R - r) * t / r);
        float y = (R - r) * std::sin(t) - p * std::sin((R - r) * t / r);

        LaserPoint pt;
        pt.x = LaserPoint::NormalizeCoord(x / maxRadius);
        pt.y = LaserPoint::NormalizeCoord(y / maxRadius);
        pt.z = 0;
        pt.r = color.r;
        pt.g = color.g;
        pt.b = color.b;
        pt.intensity = color.a;
        pt.isBlanked = false;
        points.push_back(pt);
    }
    return points;
}

std::vector<LaserPoint> LaserEngine::GenerateBeamFan(
    int beamCount,
    float angleSpread,
    const glm::vec4& color
) {
    std::vector<LaserPoint> points;
    points.reserve(beamCount * 8);

    for (int i = 0; i < beamCount; ++i) {
        float fraction = (beamCount > 1) ? (static_cast<float>(i) / static_cast<float>(beamCount - 1) - 0.5f) : 0.0f;
        float angle = fraction * angleSpread;

        float x = std::sin(angle);
        float y = std::cos(angle) - 0.5f;

        // Origin point
        LaserPoint p0;
        p0.x = 0;
        p0.y = LaserPoint::NormalizeCoord(-0.9f);
        p0.r = color.r;
        p0.g = color.g;
        p0.b = color.b;
        p0.intensity = color.a;
        points.push_back(p0);

        // Tip point
        LaserPoint p1;
        p1.x = LaserPoint::NormalizeCoord(x);
        p1.y = LaserPoint::NormalizeCoord(y);
        p1.r = color.r;
        p1.g = color.g;
        p1.b = color.b;
        p1.intensity = color.a;
        points.push_back(p1);
    }
    return points;
}

LaserTelemetry LaserEngine::TransmitPoints(
    const std::vector<LaserPoint>& points,
    int targetPPS,
    bool safetyShutter,
    int /*dacType*/
) {
    LaserTelemetry tele{};
    tele.currentPPS = targetPPS;
    tele.shutterOpen = safetyShutter;
    tele.bufferFill = 0.5f;
    tele.droppedPoints = 0;

    m_bufferedPointCount += static_cast<int>(points.size());
    return tele;
}

} // namespace nf
