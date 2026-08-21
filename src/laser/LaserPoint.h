#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <algorithm>

namespace nf {

struct LaserPoint {
    int16_t x = 0;
    int16_t y = 0;
    int16_t z = 0;
    float r = 0.0f;
    float g = 1.0f;
    float b = 0.0f;
    float intensity = 1.0f;
    bool isBlanked = false;

    static int16_t NormalizeCoord(float norm) {
        float clamped = std::clamp(norm, -1.0f, 1.0f);
        return static_cast<int16_t>(std::round(clamped * 32767.0f));
    }

    static float DenormalizeCoord(int16_t coord) {
        return static_cast<float>(coord) / 32767.0f;
    }
};

} // namespace nf
